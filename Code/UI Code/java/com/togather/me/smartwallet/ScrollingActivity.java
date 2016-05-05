package com.togather.me.smartwallet;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentSender;
import android.location.Location;
import android.os.Bundle;
import android.os.Handler;
import android.os.ParcelUuid;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.common.api.GoogleApiClient.ConnectionCallbacks;
import com.google.android.gms.common.api.GoogleApiClient.OnConnectionFailedListener;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Random;
import java.util.Set;
import java.util.UUID;

/*
*
* Author List: Aditya Nambiar, Siddarth Dutta
* Filename: ScrollingActivity.java
* Global Variables: nil
*/
public class ScrollingActivity extends AppCompatActivity implements ConnectionCallbacks,
        OnConnectionFailedListener,
        LocationListener {

    private RecyclerView busRidesView;
    private Adapter mAdapter;
    public static List<CashFlow> cashFlowList = new ArrayList<>();

    //Define a request code to send to Google Play services
    private final static int CONNECTION_FAILURE_RESOLUTION_REQUEST = 9000;
    private GoogleApiClient mGoogleApiClient;
    private LocationRequest mLocationRequest;
    private double currentLatitude;
    private double currentLongitude;

    public static TextView text;

    private static final int REQUEST_ENABLE_BT = 1;


    private String btAddress;
    private UUID id ;

    private BluetoothAdapter adapter = null;
    Handler handler = null;
    private BluetoothSocket btSocket = null;
    private BluetoothDevice btDevice = null;
    ReaderThread reader = null;
    private BtTransmission btTransmit;
    public static File cashlogs;

    protected int getLayoutId() {
        return R.layout.activity_listofitems;
    }
    /*
     * Function Name: randInt
     * Input: min, max
     * Output: random integer between min and max
     * Example Call: randInt(3, 10) returns a random number between 3 and 10
     */
    public static int randInt(int min, int max) {
        Random rand = new Random();
        // nextInt is normally exclusive of the top value,
        // so add 1 to make it inclusive
        int randomNum = rand.nextInt((max - min) + 1) + min;
        return randomNum;
    }


    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        LinearLayout llLayoutContainer = (LinearLayout) findViewById(R.id.ll_layout_container);
        View layoutView = LayoutInflater.from(this).inflate(getLayoutId(), null);
        llLayoutContainer.addView(layoutView);
        try {
            initViews();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }

    /*
     * Function Name: initViews
     * Input: nil
     * Function to inititate the view of the Activity
     */
    protected void initViews() throws FileNotFoundException {

        busRidesView = (RecyclerView) findViewById(R.id.rv_activity_listofitems);

            for (int i = 0; i < 1; i++) {

                // Sample cashflow for demo purposes.
                CashFlow a = new CashFlow();
                a.ampm = "PM";
                a.desp = "Lent to Rahul";
                a.time_hours = randInt(1, 9);
                a.time_minutes = randInt(11, 59);
                a.amt = randInt(1, 1000);

                if (i % 3 == 0) {
                    a.latitude = 40.722543;
                    a.longitude = -73.998585;
                } else if (i % 3 == 1) {
                    a.latitude = 40.7577;
                    a.longitude = -73.9857;
                } else if (i % 3 == 2) {
                    a.latitude = 40.7057;
                    a.longitude = -73.9964;
                } else if (i % 3 == 3) {
                    a.latitude = 40.7064;
                    a.longitude = -74.0094;
                }

                cashFlowList.add(a);
            }
            if (!cashlogs.exists()) {
                try {
                    cashlogs.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        try (BufferedReader br = new BufferedReader(new FileReader(cashlogs))) {
            String line;
            try {
                while (false && (line = br.readLine()) != null) {
                    String[] parts = line.split("#");
                    String part1 = parts[0];
                    String part2 = parts[1];
                    String part3 = parts[2];
                    String part4 = parts[3];
                    String part5 = parts[4];
                    String part6 = parts[5];
                    String part7 = parts[6];
                    CashFlow a = new CashFlow();
                    a.time_hours = Integer.parseInt(part1);
                    a.time_minutes = Integer.parseInt(part2);
                    a.ampm = part3;
                    a.desp = part4;
                    a.amt = Integer.parseInt(part5);
                    a.latitude = Double.parseDouble(part6);
                    a.longitude = Double.parseDouble(part7);
                    cashFlowList.add(a);

                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        initRunningRoutesList();
    }

    private void initRunningRoutesList() {
        LinearLayoutManager layoutManager = new LinearLayoutManager(this);
        busRidesView = (RecyclerView) findViewById(R.id.rv_activity_listofitems);
        busRidesView.setLayoutManager(layoutManager);
        mAdapter = new Adapter(this, cashFlowList);
        busRidesView.setItemAnimator(new DefaultItemAnimator());
        busRidesView.setAdapter(mAdapter);
        System.out.println("InitRunning");
    }
    @Override
    protected void onResume() {
        super.onResume();
        //Now lets connect to the API
        mGoogleApiClient.connect();
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.v(this.getClass().getSimpleName(), "onPause()");

        //Disconnect from API onPause()
        if (mGoogleApiClient.isConnected()) {
            LocationServices.FusedLocationApi.removeLocationUpdates(mGoogleApiClient, this);
            mGoogleApiClient.disconnect();
        }


    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_scrolling);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        String filename = "cashlogs.txt";
        text = (TextView) findViewById(R.id.textconn);
        cashlogs = new File(getApplicationContext().getFilesDir(), filename);;
        if(!cashlogs.exists())
        {
            try {
                cashlogs.createNewFile();
                System.out.println("File Created ######");
            } catch (IOException e) {
                e.printStackTrace();
            }
            // write code for saving data to the file
        }

        mGoogleApiClient = new GoogleApiClient.Builder(this)
                // The next two lines tell the new client that “this” current class will handle connection stuff
                .addConnectionCallbacks((GoogleApiClient.ConnectionCallbacks) this)
                .addOnConnectionFailedListener((GoogleApiClient.OnConnectionFailedListener) this)
                        //fourth line adds the LocationServices API endpoint from GooglePlayServices
                .addApi(LocationServices.API)
                .build();

        // Create the LocationRequest object
        mLocationRequest = LocationRequest.create()
                .setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY)
                .setInterval(10 * 1000)        // 10 seconds, in milliseconds
                .setFastestInterval(1 * 1000); // 1 second, in milliseconds
        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        Intent Control = getIntent();
        btAddress = Control.getStringExtra(BTdeviceSelect.EXTRA_DEVICE_ADDRESS);
        adapter = BluetoothAdapter.getDefaultAdapter();
        btDevice = adapter.getRemoteDevice(btAddress);
        ParcelUuid[] uuid = btDevice.getUuids();
        id = UUID.fromString(uuid[0].toString());
        System.out.println("id is " + id);
        BtConnectThread connect = new BtConnectThread();
        System.out.println("connect is " + connect);
        connect.start();

        handler = new Handler();

    }

    /**
     * If connected get lat and long
     *
     */
    @Override
    public void onConnected(Bundle bundle) {
        Location location = LocationServices.FusedLocationApi.getLastLocation(mGoogleApiClient);

        if (location == null) {
            LocationServices.FusedLocationApi.requestLocationUpdates(mGoogleApiClient, mLocationRequest, this);

        } else {
            //If everything went fine lets get latitude and longitude
            currentLatitude = location.getLatitude();
            currentLongitude = location.getLongitude();

            Toast.makeText(this, currentLatitude + " WORKS " + currentLongitude + "", Toast.LENGTH_LONG).show();
        }
    }


    @Override
    public void onConnectionSuspended(int i) {}

    @Override
    public void onConnectionFailed(ConnectionResult connectionResult) {
            /*
             * Google Play services can resolve some errors it detects.
             * If the error has a resolution, try sending an Intent to
             * start a Google Play services activity that can resolve
             * error.
             */
        if (connectionResult.hasResolution()) {
            try {
                // Start an Activity that tries to resolve the error
                connectionResult.startResolutionForResult(this, CONNECTION_FAILURE_RESOLUTION_REQUEST);
                    /*
                     * Thrown if Google Play services canceled the original
                     * PendingIntent
                     */
            } catch (IntentSender.SendIntentException e) {
                // Log the error
                e.printStackTrace();
            }
        } else {
                /*
                 * If no resolution is available, display a dialog to the
                 * user with the error.
                 */
            Log.e("Error", "Location services connection failed with code " + connectionResult.getErrorCode());
        }
    }

    /**
     * If locationChanges change lat and long
     *
     *
     * @param location
     */
    @Override
    public void onLocationChanged(Location location) {
        currentLatitude = location.getLatitude();
        currentLongitude = location.getLongitude();

        Toast.makeText(this, currentLatitude + " WORKS " + currentLongitude + "", Toast.LENGTH_LONG).show();
    }

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
                ScrollingActivity.this.runOnUiThread(new Runnable() {

                    public void run() {
                        text.setText("Status :- Established connection");

                    }
                });

            }
            catch (IOException e) {
                ScrollingActivity.this.runOnUiThread(new Runnable() {

                    public void run() {
                        text.setText("Status :- Could not establish connection");

                    }
                });

            }
            if(isConnected=true) {
                return;
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

   /*
    * This is the class that is responsible for reading data from the bluetooth connection
    * It runs a thread that is constantly listening for data
    * The delimiters used are as follows
    * 'z' => Amount was given
    * 'y' => Amount was taken
    */
    private class ReaderThread extends Thread {
        public Boolean isConnected = false;
        byte[] readBuffer;
        int readBufferPosition;
        boolean stopWorker;
        @Override
        public void run() {
            stopWorker = false;
            readBufferPosition = 0;
            readBuffer = new byte[1024];
            boolean br = false;
            if(isConnected=true) {
                String data = "";
                String time = "";
                byte bwas = 'a';
                while (!Thread.currentThread().isInterrupted() && !stopWorker) {
                    try {
                        int bytesAvailable = btTransmit.inStream.available();
                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            btTransmit.inStream.read(packetBytes);

                            for (int i = 0; i < bytesAvailable; i++) {
                                byte b = packetBytes[i];
                                System.out.println("received " + b + " " + (char)b);

                                if(b=='l') {
                                    System.out.println("Data: " + data + " Time: " + time);
                                    br = false;

                                    String temp12 = "";
                                    for(int k=0; k< data.length(); k++){
                                        if( data.charAt(k) >= '0' && data.charAt(k) <= '9')
                                            temp12 = temp12 + data.charAt(k);
                                    }
                                    int amt = Integer.parseInt(temp12);



                                    CashFlow temp = new CashFlow();
                                    temp.amt = amt;
                                    if ( bwas == 'z')
                                        temp.desp = "Given";
                                    if ( bwas == 'y')
                                        temp.desp = "Taken";

                                    DateFormat df1 = new SimpleDateFormat("HH");
                                    DateFormat df2 = new SimpleDateFormat("mm");
                                    DateFormat df3 = new SimpleDateFormat("yyyy");
                                    DateFormat df4 = new SimpleDateFormat("MM");
                                    DateFormat df5 = new SimpleDateFormat("dd");

                                    Date dateobj = new Date(Long.parseLong(time)*1000);

                                    temp.time_hours = Integer.parseInt(df1.format(dateobj));
                                    temp.time_minutes = Integer.parseInt(df2.format(dateobj));
                                    temp.date = Integer.parseInt(df5.format(dateobj));
                                    temp.month = Integer.parseInt(df4.format(dateobj));
                                    temp.year = Integer.parseInt(df3.format(dateobj));
                                    System.out.println("time: " + temp.date + " " + temp.month + " "  + temp.year);
                                    if ( temp.time_hours > 12){
                                        temp.time_hours = temp.time_hours -12;
                                        temp.ampm = "PM";
                                    } else temp.ampm = "AM";

                                    temp.longitude = currentLongitude;
                                    temp.latitude = currentLatitude;
                                    System.out.println(currentLongitude);
                                    System.out.println(currentLatitude);

                                    cashFlowList.add(temp);
                                    refresh_file();
                                    mAdapter.mItems.add(temp);
                                    ScrollingActivity.this.runOnUiThread(new Runnable() {

                                        public void run() {
                                            mAdapter.notifyDataSetChanged();
                                        }
                                    });

                                    data = "";
                                    time = "";

                                }

                                else if(!(b == 'z' || b == 'y') && !br)
                                    data = data + (char)b;



                                else if(!(b == 'z' || b == 'y') && br)
                                    time = time + (char)b;

                                else if (b == 'z'|| b =='y') {
                                    br = true;
                                    bwas = b;
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

    /*
 * Function Name: refresh_file
 * Input: nil
 * Function to to refresh the file with the data in the cashflow list in order to maintain persistency.
 */
    public static void refresh_file() throws IOException {
        PrintWriter writer = null;
        try {
            writer = new PrintWriter(cashlogs);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        writer.print("");
        writer.close();
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(cashlogs);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(fos));

        for (int i = 0; i < cashFlowList.size(); i++) {
            CashFlow temp = cashFlowList.get(i);;
            bw.write(temp.time_hours + "#"+ temp.time_minutes + "#" + temp.ampm + "#" +temp.desp+ "#" + temp.amt+ "#"+ temp.latitude +"#"+ temp.longitude);
            bw.newLine();
        }

        bw.close();
    }


    @Override
    public void onStop(){
        super.onStop();
        btTransmit.isRunning=false;
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

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if(requestCode == REQUEST_ENABLE_BT){
            if(myBluetoothAdapter.isEnabled()) {
                text.setText("Status: Enabled");
            } else {
                text.setText("Status: Disabled");
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_scrolling, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        //unregisterReceiver(bReceiver);
    }
}
