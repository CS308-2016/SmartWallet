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
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Set;


public class BTdeviceSelect extends ActionBarActivity {

    static final public String EXTRA_DEVICE_ADDRESS = "com.bluedeamons.BTdeviceSelected";

    private Button btselect;
    private ListView btList;
    private BluetoothAdapter btadapter;
    private Set<BluetoothDevice> pairedDevices;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_btdevice_select);
        btselect = (Button) findViewById(R.id.btselect);
        btList = (ListView) findViewById(R.id.btlist);
        btadapter = BluetoothAdapter.getDefaultAdapter();

        if(btadapter==null){
            Toast.makeText(getApplicationContext(), "No bluetooth adapter available", Toast.LENGTH_LONG).show();
            finish();
        }
        else{
            if (btadapter.isEnabled())
            {
                pairedDevices = btadapter.getBondedDevices();
            }
            else{
                DialogFragment newFragment = new SwitchBtDialogue();
                newFragment.show(getFragmentManager(), "noBT");
                //startActivityForResult( new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE), 1);
            }
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_btdevice_select, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.about_me) {
            me();
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
    //=============X===================X===================X===================X+=============

    public void populateList() {
        if (btadapter.isEnabled()) {
            ArrayList list = new ArrayList();
            if (pairedDevices.isEmpty()) {
                Toast.makeText(getApplicationContext(), "No paired device available!", Toast.LENGTH_LONG).show();
            } else {
                for (BluetoothDevice b : pairedDevices) {
                    list.add(b.getName() + " : " + b.getAddress());
                }
            }
            ArrayAdapter badapter = new ArrayAdapter(this, android.R.layout.simple_list_item_1, list);
            btList.setAdapter(badapter);
            btList.setOnItemClickListener(new myBtListListener());
        }
        else{
            DialogFragment newFragment = new SwitchBtDialogue();
            newFragment.show(getFragmentManager(), "noBT");
        }
    }


    //============ On Click Liseners================================================

    private class myBtListListener implements AdapterView.OnItemClickListener {
        public void onItemClick(AdapterView p,View v,int i,long id){
            String device = ((TextView)v).getText().toString();
            String deviceAddr = device.substring(device.length()-17);
            Toast.makeText(getApplicationContext(),deviceAddr,Toast.LENGTH_LONG).show();
            Intent intent = new Intent(BTdeviceSelect.this,ScrollingActivity.class);
            intent.putExtra(EXTRA_DEVICE_ADDRESS,deviceAddr);
            startActivity(intent);
        }
    }

    public void me(){
        DialogFragment d = new Me();
        d.show(getFragmentManager(),"me");
    }
    public void pairedDevices(View v){
        populateList();
    }
    public void scan(View v){
        Intent i = new Intent(this,btScan.class);
        startActivity(i);
        populateList();
    }
    public void exit_click(View V){
        DialogFragment d = new ExitClick();
        d.show(getFragmentManager(), "exit");
    }
    //==============================================================================



    //========================Dialogue Class===========================================
    public class SwitchBtDialogue extends DialogFragment {
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
    public class ExitClick extends DialogFragment {

        public Dialog onCreateDialog(Bundle savedInstanceState){
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            builder.setMessage(R.string.confirmationExit)
                    .setPositiveButton(R.string.okay, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int id) {
                            finish();
                        }
                    })
                    .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialogue, int id) {
                        }
                    });
            return builder.create();
        }
    }
    public class Me extends DialogFragment {
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            // Use the Builder class for convenient dialog construction
            AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
            LayoutInflater i = getActivity().getLayoutInflater();
            builder.setTitle(R.string.about_me);
            builder.setMessage(R.string.credits);

            // Create the AlertDialog object and return it
            return builder.create();
        }
    }
    //=======================================================================================

}
