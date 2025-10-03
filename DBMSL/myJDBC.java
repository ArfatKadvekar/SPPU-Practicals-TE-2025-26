package demo;
import java.sql.*;
public class myJDBC {
	
	public static void main(String[] args) {
		String url = "jdbc:mysql://10.10.8.119:3306/te31424_db";
		String usr= "te31424";
		String pw= "te31424";
		try {
			Class.forName("com.mysql.cj.jdbc.Driver");
			Connection conn = DriverManager.getConnection(url, usr, pw);
			System.out.println("Connection successful!");
			
			} catch(Exception e ) {System.out.print(e);}
 
	}
}
