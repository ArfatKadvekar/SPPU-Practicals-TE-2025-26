-- Assignment 2A: Employee Schema in MySQL

-- 1. Create Tables
CREATE TABLE Dept (
    Dept_id INT PRIMARY KEY,
    Dept_name VARCHAR(100) NOT NULL,
    Dept_location VARCHAR(100)
);

CREATE TABLE Employee (
    Emp_id INT AUTO_INCREMENT PRIMARY KEY,
    Dept_id INT,
    Emp_fname VARCHAR(50),
    Emp_lname VARCHAR(50),
    Emp_Position VARCHAR(50),
    Emp_salary DECIMAL(10,2),
    Emp_JoinDate DATE,
    FOREIGN KEY (Dept_id) REFERENCES Dept(Dept_id) ON DELETE CASCADE
);

CREATE TABLE Project (
    Proj_id INT PRIMARY KEY,
    Dept_id INT,
    Proj_Name VARCHAR(100),
    Proj_Location VARCHAR(100),
    Proj_cost DECIMAL(15,2),
    Proj_year INT,
    FOREIGN KEY (Dept_id) REFERENCES Dept(Dept_id) ON DELETE CASCADE
);

-- 2. Insert Sample Data
INSERT INTO Dept VALUES
(1,'Computer','Mumbai'),
(2,'IT','Pune'),
(3,'HR','Delhi'),
(4,'Finance','Bangalore');

INSERT INTO Employee (Dept_id, Emp_fname, Emp_lname, Emp_Position, Emp_salary, Emp_JoinDate) VALUES
(1,'Peter','Parker','Developer',50000,'1980-06-12'),
(2,'Harry','Potter','Analyst',60000,'1982-09-25'),
(1,'John','Doe','Manager',75000,'1990-03-15'),
(2,'Paul','Smith','Developer',55000,'1984-07-30'),
(3,'Helen','Brown','HR Executive',45000,'1987-12-01'),
(4,'Alice','White','Accountant',52000,'1983-05-19'),
(1,'Peter','Johnson','Tester',48000,'1988-10-10'),
(2,'Hank','Williams','Support',47000,'1992-11-05'),
(3,'Pamela','Anderson','HR Manager',80000,'1981-08-21'),
(4,'Paul','Walker','Finance Manager',90000,'1979-01-17');

INSERT INTO Project VALUES
(201,1,'Alpha','Pune',200000,2004),
(202,2,'Beta','Mumbai',450000,2005),
(203,1,'Gamma','Delhi',120000,2007),
(204,3,'Delta','Pune',300000,2005),
(205,2,'Epsilon','Bangalore',500000,2004),
(206,4,'Zeta','Pune',75000,2007);

-- 3. Create View, Index, Synonym (via view)
CREATE VIEW emp_dept_view AS
SELECT E.Emp_id, E.Emp_fname, E.Emp_lname, D.Dept_name
FROM Employee E
JOIN Dept D ON E.Dept_id = D.Dept_id;

CREATE INDEX idx_emp_lname ON Employee(Emp_lname);

CREATE VIEW emp_synonym AS
SELECT * FROM Employee;

-- 4. Queries

-- 1. Employees in Computer/IT with first name starting 'p' or 'h'
SELECT E.*
FROM Employee E
JOIN Dept D ON E.Dept_id = D.Dept_id
WHERE D.Dept_name IN ('Computer', 'IT')
AND (E.Emp_fname LIKE 'p%' OR E.Emp_fname LIKE 'h%');

-- 2. Number of different Employee Positions
SELECT Emp_Position, COUNT(*) AS Position_Count
FROM Employee
GROUP BY Emp_Position;

-- 3. 10% salary increase for employees joined before 1985
UPDATE Employee
SET Emp_salary = Emp_salary * 1.1
WHERE YEAR(Emp_JoinDate) < 1985;

-- 4. Delete Departments in Mumbai
DELETE FROM Dept
WHERE Dept_location = 'Mumbai';

-- 5. Project names in Pune
SELECT Proj_Name
FROM Project
WHERE Proj_Location = 'Pune';

-- 6. Projects with cost between 100000 and 500000
SELECT * 
FROM Project
WHERE Proj_cost BETWEEN 100000 AND 500000;

-- 7. Project maximum cost and average cost
SELECT MAX(Proj_cost) AS Max_Cost, AVG(Proj_cost) AS Avg_Cost
FROM Project;

-- 8. Employees with ID and Name in decreasing order of last name
SELECT Emp_id, CONCAT(Emp_fname, ' ', Emp_lname) AS Emp_Name
FROM Employee
ORDER BY Emp_lname DESC;

-- 9. Project details started in 2004, 2005, 2007
SELECT Proj_Name, Proj_Location, Proj_cost
FROM Project
WHERE Proj_year IN (2004, 2005, 2007);

-- 10. Optional: Employees in Computer/IT first name 'p' or 'h' (alternative clarity)
SELECT E.Emp_id, E.Emp_fname, E.Emp_lname, D.Dept_name
FROM Employee E
JOIN Dept D ON E.Dept_id = D.Dept_id
WHERE D.Dept_name IN ('Computer','IT')
AND (E.Emp_fname LIKE 'p%' OR E.Emp_fname LIKE 'h%');
