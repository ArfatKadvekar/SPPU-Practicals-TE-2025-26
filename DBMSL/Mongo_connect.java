package jdbc;
import org.bson.Document;
import com.mongodb.client.*;
import com.mongodb.client.MongoClients;
import com.mongodb.client.MongoCollection;
import com.mongodb.client.MongoDatabase;
import com.mongodb.client.model.Filters;


public class Mongo_connect {
	public static void main(String[] args) {
		String connectionString = "mongodb://te31424:te31424@10.10.8.119:27017/te31424_db";
		MongoClient mongoClient = MongoClients.create(connectionString);
		MongoDatabase database = mongoClient.getDatabase("te31424_db");
		MongoCollection<Document> collection = database.getCollection("my_collection");
		System.out.println("DB connected Successfuly");
		
        Document doc1 = new Document("name", "Arfat").append("age", 25).append("course", "Math");
        Document doc2 = new Document("name", "Soham").append("age", 22).append("course", "Physics");

        collection.insertMany(java.util.Arrays.asList(doc1, doc2));
        System.out.println("Inserted successfully.");
        
        collection.updateMany(
        	    new Document(),
        	    new Document("$set", new Document("age", 30))
        	);
        
		FindIterable<Document> documents = collection.find();

        System.out.println("Documents in 'my_collection':");
        for (Document doc : documents) {
            System.out.println(doc.toJson());
        }
        mongoClient.close();
	}

}

//Oct 13, 2025 10:26:21 AM com.mongodb.diagnostics.logging.JULLogger log
//INFO: Cluster created with settings {hosts=[10.10.8.119:27017], mode=SINGLE, requiredClusterType=UNKNOWN, serverSelectionTimeout='30000 ms', maxWaitQueueSize=500}
//DB connected Successfuly
//Oct 13, 2025 10:26:21 AM com.mongodb.diagnostics.logging.JULLogger log
//INFO: Cluster description not yet available. Waiting for 30000 ms before timing out
//Oct 13, 2025 10:26:21 AM com.mongodb.diagnostics.logging.JULLogger log
//INFO: Opened connection [connectionId{localValue:1, serverValue:631}] to 10.10.8.119:27017
//Oct 13, 2025 10:26:21 AM com.mongodb.diagnostics.logging.JULLogger log
//INFO: Monitor thread successfully connected to server with description ServerDescription{address=10.10.8.119:27017, type=STANDALONE, state=CONNECTED, ok=true, version=ServerVersion{versionList=[4, 2, 13]}, minWireVersion=0, maxWireVersion=8, maxDocumentSize=16777216, logicalSessionTimeoutMinutes=30, roundTripTimeNanos=2235504}
//Oct 13, 2025 10:26:21 AM com.mongodb.diagnostics.logging.JULLogger log
//INFO: Opened connection [connectionId{localValue:2, serverValue:632}] to 10.10.8.119:27017
//Inserted successfully.
//Documents in 'my_collection':
//{"_id": {"$oid": "68ec825cf1df257abcce5f47"}, "name": "Alice", "age": 30}
//{"_id": {"$oid": "68ec825cf1df257abcce5f48"}, "name": "Bob", "age": 30}
//{"_id": {"$oid": "68ec825cf1df257abcce5f49"}, "name": "Charlie", "age": 30}
//{"_id": {"$oid": "68ec8350be094c3b4e5e7396"}, "name": "Arfat", "age": 30, "course": "Math"}
//{"_id": {"$oid": "68ec8350be094c3b4e5e7397"}, "name": "Soham", "age": 30, "course": "Physics"}
//{"_id": {"$oid": "68ec83719748a830fe812ef3"}, "name": "Arfat", "age": 30, "course": "Math"}
//{"_id": {"$oid": "68ec83719748a830fe812ef4"}, "name": "Soham", "age": 30, "course": "Physics"}
//{"_id": {"$oid": "68ec85b5595d03639016b05f"}, "name": "Arfat", "age": 30, "course": "Math"}
//{"_id": {"$oid": "68ec85b5595d03639016b060"}, "name": "Soham", "age": 30, "course": "Physics"}
//{"_id": {"$oid": "68ec86757de43d688454cb27"}, "name": "Arfat", "age": 30, "course": "Math"}
//{"_id": {"$oid": "68ec86757de43d688454cb28"}, "name": "Soham", "age": 30, "course": "Physics"}
//Oct 13, 2025 10:26:21 AM com.mongodb.diagnostics.logging.JULLogger log
//INFO: Closed connection [connectionId{localValue:2, serverValue:632}] to 10.10.8.119:27017 because the pool has been closed.
//
