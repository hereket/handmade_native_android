package com.hereket.handmade_native_android;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import android.view.ViewGroup.LayoutParams;
import android.view.Gravity;


public class MainActivity extends Activity {
    static {
        System.loadLibrary("android_handmade");
    }

    public native int getData();
    TextView textView;

    public void screenTapped(View view) {
        int value = getData();
        setValue(value);
    }

    private void setValue(int value) {
        String number = String.valueOf(value);
        textView.setText(number);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_main);
        textView = (TextView) findViewById(R.id.text_container);

        int value = getData();
        setValue(value);
    }
}
