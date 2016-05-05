package com.togather.me.smartwallet;

import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.util.Log;
import android.view.Window;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.PolylineOptions;

/*
*
* Author List: Aditya Nambiar, Siddharth Dutta
* Filename: CashFlow.java
* Global Variables: nil
*/
public class GoogleMapActivity extends FragmentActivity implements GoogleMap.OnMarkerClickListener, GoogleMap.OnMarkerDragListener {

    private static LatLng place;

    // The l
    private static LatLng fromPosition = null;
    private static LatLng toPosition = null;

    private GoogleMap googleMap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setBackgroundDrawable(new ColorDrawable(0));
        setContentView(R.layout.activity_map);

        FragmentManager myFragmentManager = getSupportFragmentManager();
        SupportMapFragment mySupportMapFragment = (SupportMapFragment)myFragmentManager.findFragmentById(R.id.map);
        googleMap = mySupportMapFragment.getMap();
        googleMap.setOnMarkerClickListener(this);
        googleMap.setOnMarkerDragListener(this);

         addMarkers();
    }

    private void addMarkers() {
        if (googleMap != null) {
            place = new LatLng(getIntent().getDoubleExtra("latitude", 0.0), getIntent().getDoubleExtra("longitude", 0.0));

            // marker using custom image
            googleMap.addMarker(new MarkerOptions()
                    .position(place)
                    .title(getIntent().getStringExtra("desp"))
                    .icon(BitmapDescriptorFactory
                            .fromResource(R.mipmap.map_amarker)));

            googleMap.moveCamera(CameraUpdateFactory.newLatLngZoom(
                    place, 13));

        }
    }


    private void addLines() {
        if (googleMap != null) {
            googleMap.addPolyline((new PolylineOptions())
                    .width(5).color(Color.BLUE)
                    .geodesic(true));
        }
    }

    @Override
    public boolean onMarkerClick(Marker marker) {
        Log.i("GoogleMapActivity", "onMarkerClick");
        return false;
    }

    @Override
    public void onMarkerDrag(Marker marker) {
        // do nothing during drag
    }

    @Override
    public void onMarkerDragEnd(Marker marker) {
        toPosition = marker.getPosition();

    }

    @Override
    public void onMarkerDragStart(Marker marker) {
        fromPosition = marker.getPosition();
        Log.d(getClass().getSimpleName(), "Drag start at: " + fromPosition);
    }


}