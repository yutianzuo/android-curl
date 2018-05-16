package com.github.yutianzuo.curl_native.utils;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Environment;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Misc {
    public static final String CERT_NAME = "cacert.pem";

    static public void copyCertFile(final Context context) {
        final String str_path = getAppDir(context) + CERT_NAME;
        File cert_file = new File(str_path);
        if (cert_file.isFile()) {
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                String file_out = str_path;
                AssetManager assetManager = context.getAssets();
                int byteread = 0;
                InputStream is = null;
                FileOutputStream fs = null;
                try {
                    is = assetManager.open(CERT_NAME);
                    fs = new FileOutputStream(file_out);
                    byte[] buffer = new byte[2048];

                    while ((byteread = is.read(buffer)) != -1) {
                        fs.write(buffer, 0, byteread);
                    }

                    is.close();
                    fs.close();
                } catch (Throwable e) {

                }
            }
        }).start();
    }

    public static String getAppDir(Context context) {
        String dir = "";
        if (checkSDCard()) {
            dir = Environment.getExternalStorageDirectory().getAbsolutePath();
        } else {
            dir = context.getDir("androidcurl_private", Context.MODE_PRIVATE)
                    .getAbsolutePath();
        }
        dir = dir + File.separator + "androidcurl" + File.separator;
        makeDir(dir);

        String str_Hide_FilePath = dir + ".nomedia";
        File fHide = new File(str_Hide_FilePath);
        if (!fHide.isFile()) {
            try {
                fHide.createNewFile();
            } catch (Exception e) {
            }

        }
        return dir;
    }

    public static boolean checkSDCard() {
        String state = Environment.getExternalStorageState();

        if (Environment.MEDIA_MOUNTED.equals(state)) {
            return true;
        }
        return false;
    }

    public static void makeDir(String dir) {
        File f = new File(dir);
        if (!f.exists()) {
            f.mkdirs();
        }
    }

    public static void deleteFile(String filepath) {
        File file = new File(filepath);
        if (file.exists()) {
            if (file.isDirectory()) {
                File[] files = file.listFiles();
                if (files != null) {
                    if (files.length > 0) {
                        File[] delFiles = file.listFiles();
                        if (delFiles != null && delFiles.length > 0) {
                            for (File delFile : delFiles) {
                                deleteFile(delFile.getAbsolutePath());
                            }
                        }
                    }
                }
            }
            file.delete();
        }
    }

    static private boolean unZipFolder(String zipFileString, String outPathString) {
        boolean b_ret = true;
        try {
            ZipInputStream inZip = new ZipInputStream(new FileInputStream(zipFileString));
            ZipEntry zipEntry;
            String szName = "";
            while ((zipEntry = inZip.getNextEntry()) != null) {
                szName = zipEntry.getName();
                if (zipEntry.isDirectory()) {
                    // get the folder name of the widget
                    szName = szName.substring(0, szName.length() - 1);
                    File folder = new File(outPathString + File.separator + szName);
                    folder.mkdirs();
                } else {
                    int n_last_index = szName.lastIndexOf("/");
                    if (n_last_index != -1) {
                        String str_folder_name = outPathString + File.separator + szName.substring(0, n_last_index);
                        File folder = new File(str_folder_name);
                        if (!folder.isDirectory()) {
                            folder.mkdirs();
                        }
                    }
                    File file = new File(outPathString + File.separator + szName);
                    file.createNewFile();
                    // get the output stream of the file
                    FileOutputStream out = new FileOutputStream(file);
                    int len;
                    byte[] buffer = new byte[1024];
                    // read (len) bytes into buffer
                    while ((len = inZip.read(buffer)) != -1) {
                        // write (len) byte from buffer at the position 0
                        out.write(buffer, 0, len);
                        out.flush();
                    }
                    out.close();
                }
            }
            inZip.close();
        } catch (Throwable e) {
            b_ret = false;
        }
        return b_ret;
    }
}
