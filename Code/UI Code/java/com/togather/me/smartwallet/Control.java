/*
* Copyright 2015-2018 Sidhin S Thomas
*
* Author: SIDHIN S THOMAS
* GitHub UserName: ParadoxZero
* Email: sidhin.thomas@gmail.com
*
* This program is free to use and edit protected under Open GNU license agreement
*
 */


package com.togather.me.smartwallet;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;


public class Control extends ActionBarActivity {

    private Button led ;
    private String btAddress;

    private UUID id ;

    private BluetoothAdapter adapter = null;
    private BluetoothDevice btDevice = null;
    private BluetoothSocket btSocket = null;

    private BtTransmission btTransmit;

    private Boolean sendSuccess ;
    private Boolean readSuccess ;
    private Boolean ledState = false;
    Handler handler = null;
    ReaderThread reader = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_control);
        led = (Button) findViewById(R.id.led);
        Intent Control = getIntent();
        btAddress = Control.getStringExtra(BTdeviceSelect.EXTRA_DEVICE_ADDRESS);
        adapter = BluetoothAdapter.getDefaultAdapter();
        btDevice = adapter.getRemoteDevice(btAddress);
        ParcelUuid[] uuid = btDevice.getUuids();
        id = UUID.fromString(uuid[0].toString());
        BtConnectThread connect = new BtConnectThread();
        connect.start();

        handler = new Handler();
        //ConnectBT connect = new ConnectBT();
        //connect.execute();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_control, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings)
        {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
    @Override
    public void onStop(){
        super.onStop();
        btTransmit.isRunning=false;
        showMsg("Stopping!");
        btTransmit.isRunning=false;
        if(btSocket != null){
            try {
                btSocket.close();
            } catch (IOException e){
                System.out.println("loc 3");

            }
            btSocket = null;
        }

    }

    private void showMsg(String s){
        Toast.makeText(getApplicationContext(), s, Toast.LENGTH_SHORT).show();

    }
    private void goBack(){
        try {
            btSocket.close();
        } catch (IOException e){
            System.out.println("loc 2");
        }
        Intent intent = new Intent(Control.this,BTdeviceSelect.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        startActivity(intent);
    }
    //============================ On Click Listeners ===================================
    public void led_onclick(View v){
        if(btSocket.isConnected() && btSocket!=null) {
            if (ledState) {
                ledState = false;
                btTransmit.write("S".getBytes());
                System.out.println("Printing S");
            } else {
                ledState = true;

                btTransmit.write("B".getBytes());
                System.out.println("Printing B");

            }
        }
        else {
            showMsg("Not connected, please try again!");
        }
    }
    public void me(View v){
        DialogFragment d = new Me();
        d.show(getFragmentManager(),"me");
    }
    public void disconnect(View v){
        goBack();
    }
    //================================================================================

    //========================Threads=================================================
    private class BtConnectThread extends Thread {
        public Boolean isConnected = false;
        @Override
        public void run(){


            try {
                btSocket = btDevice.createInsecureRfcommSocketToServiceRecord(id);
                btTransmit = new BtTransmission();
                btTransmit.inStream = btSocket.getInputStream();
                btTransmit.outStream = btSocket.getOutputStream();
                btTransmit.start();

                adapter.cancelDiscovery();
                btSocket.connect();
                isConnected = true;
                reader = new ReaderThread();
                reader.start();
            }
            catch (IOException e) {
                System.out.println("loc 1");
                System.out.println("loc 1:"+btTransmit.inStream);
                System.out.println("loc 1:"+btTransmit.outStream);
                goBack();
            }
            if(isConnected=true) {
//                System.out.println("listening starting");
                return;
            }
        }

    }

    private class ReaderThread extends Thread {
        public Boolean isConnected = false;
        Thread workerThread;
        byte[] readBuffer;
        int readBufferPosition;
        int counter;
        boolean stopWorker;
        @Override
        public void run() {
//            final Handler handler = new Handler();
            final byte delimiter = 10; //This is the ASCII code for a newline character



            stopWorker = false;
            readBufferPosition = 0;
            readBuffer = new byte[1024];
            if(isConnected=true) {
                System.out.println("listening starting");
                while (!Thread.currentThread().isInterrupted() && !stopWorker) {
                    try {
                        int bytesAvailable = btTransmit.inStream.available();
                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            btTransmit.inStream.read(packetBytes);
                            for (int i = 0; i < bytesAvailable; i++) {
                                byte b = packetBytes[i];
                                System.out.println("received " + b + " " + (char)b);
                                if (b == delimiter) {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "US-ASCII");
                                    readBufferPosition = 0;

                                    handler.post(new Runnable() {
                                        public void run() {
                                            System.out.println("FUCK YEAHH " + data);
                                            // text.setText(data);
                                        }
                                    });
                                } else {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                        }
                    } catch (IOException ex) {
                        System.out.println("loc 4");

                        stopWorker = true;
                    }
                }
            }
            else {

                System.out.println("not connected yet");
            }
        }
    }






    private class BtTransmission extends Thread {

        private InputStream inStream;
        private OutputStream outStream;;
        public Boolean isRunning = true ;
        private byte[] Write ;
        public void run() {
            if (Write != null) {
                try {
                    outStream.write(Write);
                    sendSuccess = true;
                    Write = null;
                } catch (IOException e) {
                    System.out.println("loc 5");

                    sendSuccess = false;
                }
            }
            if(isRunning=false) return;

        }


        public void write(byte[] w){
                Write = w;
        }
        public void cancel() {
                try {
                    btSocket.close();
                } catch (IOException e) {
                    System.out.println("loc 6");

                }
        }

    }

    //==========================================================================

    //=================================Dialogs==================================

    public class Me extends DialogFragment {
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            // Use the Builder class for convenient dialog construction
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setMessage(R.string.switchOnBT)
                    .setPositiveButton("Okay", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            finish();
                        }
                    });
            // Create the AlertDialog object and return it
            return builder.create();
        }
    }
    //==========================================================================
}
