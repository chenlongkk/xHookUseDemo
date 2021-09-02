package com.cck.xhook;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.view.View;
import android.widget.Toast;

import com.cck.io.monitor.Config;
import com.cck.io.monitor.IOMonitor;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private Handler mHandle = new Handler(Looper.getMainLooper());
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        IOMonitor.install(new Config(true, 0,0,0,0,0,0));
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testWriteFile();
            }
        });
        findViewById(R.id.button2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                testReadFile();
            }
        });
    }

    private void testReadFile() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    BufferedReader br = new BufferedReader(new FileReader(new File(getExternalFilesDir(null), "test.txt")));
                    String s = br.readLine();
                    br.close();
                    mHandle.post(new Runnable() {
                        @Override
                        public void run() {
                            Toast.makeText(MainActivity.this, s, Toast.LENGTH_SHORT).show();
                        }
                    });
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }

    private void testWriteFile() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    File s = new File(getExternalFilesDir(null), "test.txt");
                    BufferedWriter bw = new BufferedWriter(new FileWriter(s));
                    for (int i = 0; i < 1024; i++) {
                        bw.write("hahahahh\n");
                    }
                    bw.flush();
                    bw.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }).start();
    }
}