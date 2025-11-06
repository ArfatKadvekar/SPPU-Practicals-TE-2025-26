package mongo;
import com.mongodb.client.*;
import org.bson.Document;
import java.util.*;

public class Mongoconn {
	 public static void main(String[] args) {
		 // String connectionString = "mongodb://te31424:te31424@10.10.8.119:27017/te31424_db";
	        try (MongoClient mongoClient = MongoClients.create("mongodb://localhost:27017")) {
	            MongoDatabase db = mongoClient.getDatabase("studentDB");
	            MongoCollection<Document> collection = db.getCollection("students");
	            
	            Scanner sc = new Scanner(System.in);

	            while (true) {
	                System.out.println("\n=== MongoDB Menu ===");
	                System.out.println("1. Add Record");
	                System.out.println("2. View Records");
	                System.out.println("3. Edit Record");
	                System.out.println("4. Delete Record");
	                System.out.println("5. Exit");
	                System.out.print("Enter choice: ");
	                int choice = sc.nextInt();
	                sc.nextLine(); // consume newline

	                if (choice == 1) { // Add
	                    System.out.print("Enter name: ");
	                    String name = sc.nextLine();
	                    System.out.print("Enter subject: ");
	                    String subject = sc.nextLine();
	               		Document doc = new Document();   // Create an empty document
	                    doc.put("name", name);           // Add field "name"
	                    doc.put("subject", subject); 
	                    collection.insertOne(doc);
	                    collection.insertOne(doc);
	                    System.out.println("Record inserted.");

	                } else if (choice == 2) { // View
	                    System.out.println(" Records:");
	                    for (Document d : collection.find()) {
	                        System.out.println(d.toJson());
	                    }

	                } else if (choice == 3) { // Edit
	                    System.out.print("Enter name to update: ");
	                    String name = sc.nextLine();
	                    System.out.print("Enter new subject: ");
	                    String newSub = sc.nextLine();
	                    collection.updateOne(
	                            new Document("name", name),
	                            new Document("$set", new Document("subject", newSub))
	                    );
	                    System.out.println("Record updated.");

	                } else if (choice == 4) { // Delete
	                    System.out.print("Enter name to delete: ");
	                    String name = sc.nextLine();
	                    collection.deleteOne(new Document("name", name));
	                    System.out.println("Record deleted.");

	                } else if (choice == 5) {
	                    System.out.println("Exiting...");
	                    break;
	                } else {
	                    System.out.println("Invalid choice!");
	                }
	            }
	        } catch (Exception e) {
	            e.printStackTrace();
	        }
    }
}
