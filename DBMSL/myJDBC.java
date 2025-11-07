package demo;
import java.sql.*;
import java.util.Scanner;

public class JDBCAssignment {
    static final String DB = "jdbc:mysql://localhost/arfat_db";
    static final String DRIVER = "com.mysql.cj.jdbc.Driver";
    static final String USER = "root";
    static final String PASS = "*****";

    private static Connection connectDatabase() throws ClassNotFoundException, SQLException {
        Class.forName(DRIVER);
        return DriverManager.getConnection(DB, USER, PASS);
    }

    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        try (Connection con = connectDatabase()) {
            while (true) {
                System.out.println("\n-------------Menu------------------");
                System.out.println("1. Insert Student");
                System.out.println("2. Update Student");
                System.out.println("3. Show Students with > 65%");
                System.out.println("4. Delete Student");
                System.out.println("0. Exit");
                System.out.print("Choose an option: ");
                int choice = sc.nextInt();
                sc.nextLine();

                switch (choice) {
                    case 1:
                        System.out.print("Roll No: ");
                        int roll = sc.nextInt();
                        sc.nextLine();
                        System.out.print("Name: ");
                        String name = sc.nextLine();
                        System.out.print("Percentage: ");
                        float perc = sc.nextFloat();
                        try (PreparedStatement ps = con.prepareStatement("INSERT INTO Student VALUES (?, ?, ?)")) {
                            ps.setInt(1, roll);
                            ps.setString(2, name);
                            ps.setFloat(3, perc);
                            System.out.println(ps.executeUpdate() + " record(s) inserted");
                        }
                        break;

                    case 2:
                        System.out.print("Update roll number or name? (roll/name): ");
                        String col = sc.nextLine();
                        if (col.equalsIgnoreCase("roll")) {
                            System.out.print("New Roll No: ");
                            int newRoll = sc.nextInt();
                            sc.nextLine();
                            System.out.print("Where Name: ");
                            String whereName = sc.nextLine();
                            String sql = "UPDATE Student SET rollno=? WHERE name=?";
                            try (PreparedStatement ps = con.prepareStatement(sql)) {
                                ps.setInt(1, newRoll);
                                ps.setString(2, whereName);
                                System.out.println(ps.executeUpdate() + " updated");
                            }
                        } else {
                            System.out.print("New Name: ");
                            String newName = sc.nextLine();
                            System.out.print("Where Name: ");
                            String whereName2 = sc.nextLine();
                            String sql = "UPDATE Student SET name=? WHERE name=?";
                            try (PreparedStatement ps = con.prepareStatement(sql)) {
                                ps.setString(1, newName);
                                ps.setString(2, whereName2);
                                System.out.println(ps.executeUpdate() + " updated");
                            }
                        }
                        break;

                    case 4:
                        System.out.print("Delete by roll number: ");
                        int delRoll = sc.nextInt();
                        try (PreparedStatement ps = con.prepareStatement("DELETE FROM Student WHERE rollno=?")) {
                            ps.setInt(1, delRoll);
                            System.out.println(ps.executeUpdate() + " deleted");
                        }
                        break;
                    case 3:
                        Statement stmt2 = con.createStatement();
                        ResultSet rs = stmt2.executeQuery("SELECT * FROM Student WHERE percentage > 65.0");
                        System.out.println("Roll\tName\tPercentage");
                        while (rs.next()) {
                            System.out.println(rs.getInt(1) + "\t" + rs.getString(2) + "\t" + rs.getFloat(3));
                        }
                        rs.close();
                        break;

                    case 0:
                        System.out.println("Exiting.");
                        return;
                    default:
                        System.out.println("Invalid option. Try again.");
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        sc.close();
    }
}
