-- ================================
-- ASSIGNMENT 2A & 2B : STUDENT SCHEMA (MySQL)
-- ================================

-- 1. Create Tables
CREATE TABLE PlacementDrive (
    Drive_id INT PRIMARY KEY,
    Pcompany_name VARCHAR(100) NOT NULL,
    package INT,
    location VARCHAR(100)
);

CREATE TABLE Training (
    T_id INT PRIMARY KEY,
    Tcompany_name VARCHAR(100),
    T_Fee DECIMAL(10,2),
    T_year INT
);

CREATE TABLE Student (
    s_id INT AUTO_INCREMENT PRIMARY KEY,
    Drive_id INT,
    T_id INT,
    s_name VARCHAR(100),
    CGPA DECIMAL(3,2),
    s_branch VARCHAR(50),
    s_dob DATE,
    FOREIGN KEY (Drive_id) REFERENCES PlacementDrive(Drive_id) ON DELETE CASCADE,
    FOREIGN KEY (T_id) REFERENCES Training(T_id) ON DELETE CASCADE
);

-- 2. Insert Sample Data
INSERT INTO PlacementDrive VALUES
(1,'Infosys',30000,'Pune'),
(2,'TCS',40000,'Mumbai'),
(3,'Wipro',35000,'Delhi'),
(4,'Capgemini',50000,'Bangalore'),
(5,'Tech Mahindra',45000,'Pune');

INSERT INTO Training VALUES
(1,'ABC Training',15000,2018),
(2,'TechLearn',18000,2019),
(3,'SkillUp',20000,2020),
(4,'CodeBoost',22000,2021),
(5,'LearnHub',25000,2019);

INSERT INTO Student (Drive_id, T_id, s_name, CGPA, s_branch, s_dob) VALUES
(1,1,'Amit Sharma',8.5,'Computer','2000-01-15'),
(2,2,'Deepa Patil',7.9,'IT','2001-04-12'),
(3,3,'Rahul Mehta',6.8,'Civil','1999-03-21'),
(4,4,'Divya Rao',9.2,'Computer','2000-07-19'),
(5,5,'Arjun Nair',7.4,'IT','2001-08-23'),
(1,2,'Dinesh Kumar',8.1,'Mechanical','1999-11-02'),
(2,3,'Anjali Singh',9.5,'Computer','2000-05-10'),
(3,1,'Dharma Patel',6.5,'IT','2001-09-29'),
(4,5,'Aditya Joshi',8.8,'Computer','2000-10-08'),
(5,4,'Diya Verma',7.0,'IT','2001-12-11');

-- 3. Create View, Index, and Synonym (via view)
CREATE VIEW student_view AS
SELECT S.s_id, S.s_name, S.s_branch, T.Tcompany_name, D.Pcompany_name
FROM Student S
JOIN Training T ON S.T_id = T.T_id
JOIN PlacementDrive D ON S.Drive_id = D.Drive_id;

CREATE INDEX idx_student_name ON Student(s_name);

-- Check index
SHOW INDEXES FROM Student;

-- Verify index used
EXPLAIN SELECT * FROM Student WHERE s_name = 'Amit Sharma';

-- “Sequence” handled by AUTO_INCREMENT (MySQL style)
-- “Synonym” as view
CREATE VIEW student_synonym AS
SELECT * FROM Student;

-- ================================
-- 4. QUERIES (Assignment 2B)
-- ================================

-- 1️⃣ Display all student details with branch ‘Computer’ and ‘IT’ and name starting with 'a' or 'd'
SELECT *
FROM Student
WHERE s_branch IN ('Computer','IT')
AND (s_name LIKE 'a%' OR s_name LIKE 'd%');

-- 2️⃣ List the number of different companies (using DISTINCT)
SELECT COUNT(DISTINCT Pcompany_name) AS Different_Companies
FROM PlacementDrive;

-- 3️⃣ Give 15% increase in fee of Training whose year is 2019
UPDATE Training
SET T_Fee = T_Fee * 1.15
WHERE T_year = 2019;

-- 4️⃣ Delete Student details having CGPA less than 7
DELETE FROM Student
WHERE CGPA < 7;

-- 5️⃣ Find company names belonging to Pune or Mumbai
SELECT Pcompany_name
FROM PlacementDrive
WHERE location IN ('Pune','Mumbai');

-- 6️⃣ Find student names who joined training in 1-1-2019 and also in 1-1-2021
-- (Assuming joining year matches training year)
SELECT S.s_name
FROM Student S
JOIN Training T ON S.T_id = T.T_id
WHERE T.T_year IN (2019, 2021);

-- 7️⃣ Find student with maximum CGPA and those with CGPA between 7 to 9
SELECT s_name AS Max_CGPA_Student
FROM Student
WHERE CGPA = (SELECT MAX(CGPA) FROM Student);

SELECT s_name AS CGPA_Between_7_9
FROM Student
WHERE CGPA BETWEEN 7 AND 9;

-- 8️⃣ Display all students with T_id and decreasing order of Fees
SELECT S.s_name, S.T_id, T.T_Fee
FROM Student S
JOIN Training T ON S.T_id = T.T_id
ORDER BY T.T_Fee DESC;

-- 9️⃣ Display Pcompany name, S_name, location and Package for Package = 30K, 40K, 50K
SELECT D.Pcompany_name, S.s_name, D.location, D.package
FROM Student S
JOIN PlacementDrive D ON S.Drive_id = D.Drive_id
WHERE D.package IN (30000,40000,50000);
