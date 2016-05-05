package com.togather.me.smartwallet;

import android.content.Context;
import android.content.Intent;
import android.support.v7.widget.RecyclerView;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageButton;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
/*
*
* Author List: Aditya Nambiar, Siddharth Dutta
* Filename: Adapter.java
* Functions: onCreateViewHolder, onBindViewHolder, getItem, getItemCount, onClick
* Global Variables: nil
*/
public class Adapter extends RecyclerView.Adapter<Adapter.ItemHolder> {
    Context mContext;
    // This lists holds all cashflows that are displayed in the app
    List<CashFlow> mItems;
    LayoutInflater mInflater;
    private Edit edit_dialog;

    public Adapter(Context context, List<CashFlow> items ) {
        this.mContext = context;
        mItems = new ArrayList<CashFlow>();
        mItems.addAll(items);
        this.mInflater = LayoutInflater.from(context);
        notifyDataSetChanged();
    }

    @Override
    public ItemHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = mInflater.inflate(R.layout.cashflowcard, parent, false);
        return new ItemHolder(view);
    }

    @Override
    public void onBindViewHolder(ItemHolder holder, int position) {
         CashFlow temp = mItems.get(position);
        holder.time.setText(String.valueOf(temp.time_hours) + ": " + String.valueOf(temp.time_minutes) + temp.ampm);
        holder.date.setText(temp.date + "/"+temp.month+"/"+temp.year);
        holder.desp.setText(temp.desp);
        holder.amt.setText(String.valueOf(temp.amt));
        ImageButton img_button = (ImageButton)  holder.itemView.findViewById(R.id.locButton);
        img_button.setTag(position); //For passing the list item index
        final int pos = position;
        img_button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                CashFlow temp = mItems.get(pos);
                Intent intent = new Intent(mContext, GoogleMapActivity.class);
                intent.putExtra("longitude", temp.longitude);
                intent.putExtra("latitude", temp.latitude);
                mContext.startActivity(intent);
            }
        });
    }

    // Returns the Cashflow item at postion @param position
    public CashFlow getItem(int position) {
        return mItems.get(position);
    }



    @Override
    public int getItemCount() {
        return mItems.size();
    }

    public class ItemHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
        TextView time;
        TextView desp;
        TextView amt;
        TextView date;


        public ItemHolder(View itemView) {
            super(itemView);
            time = (TextView) itemView.findViewById(R.id.timing);
            desp = (TextView) itemView.findViewById(R.id.desp);
            amt = (TextView) itemView.findViewById(R.id.amt);
            date = (TextView) itemView.findViewById(R.id.date);
            itemView.setOnClickListener(this);
        }
        
        /* When a items in the list of cashflows is clicked the below function is called.
         * It opends the edit_dialog window to edit the information about the cashflow.
         */
        @Override
        public void onClick(View v) {

            CashFlow item = getItem(getAdapterPosition());
            edit_dialog = new Edit(mContext,getAdapterPosition() );
            edit_dialog.setCancelable(false);
            edit_dialog.setCanceledOnTouchOutside(false);
            edit_dialog.setCashFlow(item, Adapter.this);
            edit_dialog.show();
            Window window = edit_dialog.getWindow();
            DisplayMetrics displaymetrics = new DisplayMetrics();
            WindowManager wm = (WindowManager)mContext.getSystemService(Context.WINDOW_SERVICE);
            Display display = wm.getDefaultDisplay();
            display.getMetrics(displaymetrics);
            int height = (int) (displaymetrics.heightPixels * 0.8);

            window.setLayout(WindowManager.LayoutParams.MATCH_PARENT, height);

        }

    }

}
