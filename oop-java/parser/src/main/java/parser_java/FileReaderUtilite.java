package parser_java;

import java.io.*;

public class  FileReaderUtilite{
    public static String fileread(String path){
        StringBuilder result = new StringBuilder();
        try(BufferedReader br = new BufferedReader(new FileReader(path))){
            String line;
            while ((line = br.readLine())!=null){
                result.append(line).append("\n");
            }
        }catch (IOException eror){
            eror.printStackTrace();
        }
        return result.toString();
    }
}
