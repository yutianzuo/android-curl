package com.github.yutianzuo.myapplication;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.github.yutianzuo.curl_native.JniCurl;
import com.github.yutianzuo.curl_native.utils.Misc;
import com.github.yutianzuo.myapplication.BizNetWrapper.HttpCallbackBiz;
import com.github.yutianzuo.native_crash_handler.NativeCrashHandler;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    TextView mTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        NativeCrashHandler.installNativeCrashHandler("/sdcard/curlcrash.log");

        BizNetWrapper.INSTANCE.init(this);
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button btnGet = findViewById(R.id.get);
        Button btnGet20 = findViewById(R.id.gethttp20);
        Button btnPostForm = findViewById(R.id.postform);
        Button btnPostJson = findViewById(R.id.postjson);
        Button btnPut = findViewById(R.id.put);
        Button btnPostFile = findViewById(R.id.postfile);
        Button btnDownload = findViewById(R.id.download);


        mTextView = findViewById(R.id.txt_info);

        btnGet.setOnClickListener(this);
        btnPostForm.setOnClickListener(this);
        btnPostJson.setOnClickListener(this);
        btnPut.setOnClickListener(this);
        btnPostFile.setOnClickListener(this);
        btnDownload.setOnClickListener(this);
        btnGet20.setOnClickListener(this);

        cryptoTestcase();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        BizNetWrapper.INSTANCE.uninit();
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.gethttp20:
                testCaseGet20();
                break;
            case R.id.get:
                testCaseGet();
                break;
            case R.id.postform:
                testCasePostForm();
                break;
            case R.id.postjson:
                testCasePostJson();
                break;
            case R.id.put:
                testCasePutJson();
                break;
            case R.id.postfile:
                testCasePostFile();
                break;
            case R.id.download:
                testCaseDownload();
                break;
        }
    }

    private void testCaseGet20() {
        mTextView.setText("Get TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager20(MainActivity.this)).
        get(
        new HttpCallbackBiz() {
            @Override
            public void success(BeanTest data) {
                mTextView.setText("Get TestCase OK");
                //Log.e("JAVA_TAG", data.rep);
            }

            @Override
            public void fail(int errcode) {
                mTextView.setText("Get TestCase Failed:" + errcode);
                Log.e("JAVA_TAG", "fail:" + errcode);
            }

            @Override
            public void progress(float persent) {
                Log.e("JAVA_TAG", "progress");
            }
        });
    }

    private void testCaseGet() {
        mTextView.setText("Get TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager(MainActivity.this)).
                addHeader("customheader1", "value").
                addHeader("customheader2", "value").
                addUrlParam("customparam1", "value").
                addUrlParam("customparam2", "value").
//                setPath("567/234").
                get(new HttpCallbackBiz() {
                    @Override
                    public void success(BeanTest data) {
                        mTextView.setText("Get TestCase OK");
                        Log.e("JAVA_TAG", data.rep);
                    }

                    @Override
                    public void fail(int errcode) {
                        mTextView.setText("Get TestCase Failed:" + errcode);
                        Log.e("JAVA_TAG", "fail:" + errcode);
                    }

                    @Override
                    public void progress(float persent) {
                        Log.e("JAVA_TAG", "progress");
                    }
                });
    }

    private void testCasePostForm() {
        mTextView.setText("PostForm TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager2(MainActivity.this)).
                setPath("").
                addFormData("postkey1", "postdata1").
                addFormData("postkey2", "postdata2").
                postFormdata(new HttpCallbackBiz() {
                    @Override
                    public void success(BeanTest data) {
                        mTextView.setText("PostForm TestCase OK");
                        Log.e("JAVA_TAG", data.rep);
                    }

                    @Override
                    public void fail(int errcode) {
                        mTextView.setText("PostForm TestCase Failed:" + errcode);
                    }

                    @Override
                    public void progress(float persent) {
                        Log.e("JAVA_TAG", "progress");
                    }
                });
    }

    private void testCasePostJson() {
        mTextView.setText("PostJson TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager2(MainActivity.this)).
                setPath("").
                setJson("{\"key\":\"value\"}").
                postJson(new HttpCallbackBiz() {
                    @Override
                    public void success(BeanTest data) {
                        mTextView.setText("PostJson TestCase OK");
                        Log.e("JAVA_TAG", data.rep);
                    }

                    @Override
                    public void fail(int errcode) {
                        mTextView.setText("PostJson TestCase Failed:" + errcode);
                    }

                    @Override
                    public void progress(float persent) {
                        Log.e("JAVA_TAG", "progress");
                    }
                });
    }

    private void testCasePutJson() {
        mTextView.setText("PutJson TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager2(MainActivity.this)).
                setPath("api/path3").
                setJson("{\"key\":\"value\"}").
                putJson(new HttpCallbackBiz() {
                    @Override
                    public void success(BeanTest data) {
                        mTextView.setText("PutJson TestCase OK");
                        Log.e("JAVA_TAG", data.rep);
                    }

                    @Override
                    public void fail(int errcode) {
                        mTextView.setText("PutJson TestCase Failed:" + errcode);
                    }

                    @Override
                    public void progress(float persent) {
                        Log.e("JAVA_TAG", "progress");
                    }
                });
    }

    private void testCasePostFile() {
        mTextView.setText("PostFile TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager2(MainActivity.this)).
                setPath("upload.cgi").
//                setJsonName("jsonName").
//                setJson("{\"key\":\"value\"}").
//                setFormName("formName").
//                addFormData("a", "b").
//                addFormData("c", "d").
                setFileKeyName("file").
                setFileName("testfile.file").
                setFilePath(Misc.getAppDir(MainActivity.this) + Misc.CERT_NAME).
                postFile(new HttpCallbackBiz() {
                    @Override
                    public void success(BeanTest data) {
                        mTextView.setText("PostFile TestCase OK");
                        Log.e("JAVA_TAG", data.rep);
                    }

                    @Override
                    public void fail(int errcode) {
                        mTextView.setText("PostFile TestCase Failed:" + errcode);
                    }

                    @Override
                    public void progress(float persent) {
                        mTextView.setText("PostFile TestCase Progress:" + persent + "%");
                        Log.e("JAVA_TAG", "progress");
                    }
                });
    }

    private void testCaseDownload() {
        mTextView.setText("Download TestCase Begins");
        new BizNetWrapper.UrlBuilder().
                with(BizNetWrapper.INSTANCE.getBizRequestManager3(MainActivity.this)).
//                addHeader("customheader1", "value").
//                addHeader("customheader2", "value").
//                addUrlParam("customparam1", "value").
//                addUrlParam("customparam2", "value").
                setDownloadFilePath(Misc.getAppDir(MainActivity.this) + "boss.apk").
                downloadFile(new HttpCallbackBiz() {
                    @Override
                    public void success(BeanTest data) {
                        mTextView.setText("Download TestCase OK");
                        Log.e("JAVA_TAG", data.rep);
                    }

                    @Override
                    public void fail(int errcode) {
                        mTextView.setText("Download TestCase Failed:" + errcode);
                        Log.e("JAVA_TAG", "fail:" + errcode);
                    }

                    @Override
                    public void progress(float persent) {
                        mTextView.setText("Download TestCase Progress:" + persent + "%");
                        Log.e("JAVA_TAG", "progress");
                    }
                });
    }

    private void cryptoTestcase() {
        //a demo java aes-encryt data and openssl aes-deccryt data
        byte[] aseByte = new Crypto().aesEncrypt("0123456789abcdef", "fedcba9876543210", "AndroidTestcase12345678");
        String strDecryptString = JniCurl.aesCbc(aseByte, aseByte.length, "0123456789abcdef", "fedcba9876543210");

        String strSha = JniCurl.sha256("abcd");
        strSha = "cpp:" + strSha;
        String strShaJava = new Crypto().SHA256("abcd");
        strShaJava = "java:" + strShaJava;
        mTextView.setText(strSha);
    }
}
