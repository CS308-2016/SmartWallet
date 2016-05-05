package com.togather.me.smartwallet;

import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Vibrator;
import android.util.Log;
import android.widget.Toast;


public class MyBroadcastReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(Context context, Intent intent) {
        String result = intent.getAction();
        if(BluetoothDevice.ACTION_FOUND.equals(result)){
            BluetoothDevice b = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
            if(b!=null) {

                Log.e("siddhartha", "i was here" + b.getName());
                int  rssi = intent.getShortExtra(BluetoothDevice.EXTRA_RSSI,Short.MIN_VALUE);
                Log.e("siddhartha", "rssi: " + rssi);
            }
        }
        else if (BluetoothDevice.ACTION_ACL_DISCONNECT_REQUESTED.equals(result)) {
            System.out.println("disconnect request");
            Log.d("siddhartha", "device disconnect request");
        }
        else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(result)) {
            System.out.println("disconnected device");
            Log.d("siddhartha", "device disconnected");
            Toast.makeText(context, "Connection lost!",
                    Toast.LENGTH_LONG).show();
            Vibrator vibrator = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
            vibrator.vibrate(2000);
            ScrollingActivity.text.setText("Status :- Connection Lost!");
        }
    }

}