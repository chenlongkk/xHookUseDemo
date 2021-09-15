package com.cck.xhook;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import com.cck.io.monitor.Config;
import com.cck.io.monitor.IOMonitor;
import com.cck.io.monitor.XLog;
import com.cck.io.monitor.XLogConfig;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

public class MainActivity extends AppCompatActivity {

    private Handler mHandle = new Handler(Looper.getMainLooper());
    private XLog mXLog;
    private Random mRandom = new Random();
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mXLog = new XLog(new XLogConfig(getFilesDir().getAbsolutePath()));
        IOMonitor.install(new Config(true));
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startTestLog();
            }
        });
        findViewById(R.id.button2).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                testReadFile();
                mXLog.flush();
            }
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.d("chenlong", "onDestroy");
        mXLog.release();
    }

    private void startTestLog() {
        new Thread(new Runnable() {
            @Override
            public void run() {
                for (int i = 0; i < 5; i++) {
                    final int idx = i;
                    Thread s = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            for (int j = 0; j < 100; j++) {
                                mXLog.write(buildRandomString(mRandom.nextInt(30)));
                            }
                            Log.d("chenlong2", "finish, " + idx);
                        }
                    });
                    s.start();

                }
            }

        }).start();

    }

    private String buildRandomString(int size) {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < size; i++) {
            sb.append(mRandom.nextInt(127));
        }
        return sb.toString();
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