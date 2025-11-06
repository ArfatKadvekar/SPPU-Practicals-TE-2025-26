-- ======================================================
-- ASSIGNMENT 3  (BASED ON EMPLOYEE SCHEMA)
-- ======================================================

-- SCHEMA:
-- Employee(Emp_id, Dept_id, Emp_fname, Emp_lname, Emp_Position,
--          Emp_salary, Emp_JoinDate)
-- Dept(Dept_id, Dept_name, location)
-- Project(Proj_id, Dept_id, Proj_Name, Proj_Location, Proj_cost, Proj_year)

-- ======================================================
-- 1️⃣  Find Employee details and Department details using NATURAL JOIN
-- ======================================================
SELECT *
FROM Employee
NATURAL JOIN Dept;

-- ======================================================
-- 2️⃣  Find emp_fname, Emp_position, location, Emp_JoinDate
--      who have same Dept_id
--      → Use INNER JOIN
-- ======================================================
SELECT E.Emp_fname, E.Emp_Position, D.location, E.Emp_JoinDate
FROM Employee E
INNER JOIN Dept D ON E.Dept_id = D.Dept_id;

-- ======================================================
-- 3️⃣  Find Employee details, Proj_id, Project cost
--      who do not have Project Location = 'Hyderabad'
--      → Use INNER JOIN with WHERE NOT condition
-- ======================================================
SELECT E.*, P.Proj_id, P.Proj_cost, P.Proj_Location
FROM Employee E
INNER JOIN Project P ON E.Dept_id = P.Dept_id
WHERE P.Proj_Location <> 'Hyderabad';

-- ======================================================
-- 4️⃣  Find Department Name, Employee Name, Emp_Position
--      for which Project year = 2020
--      → Use MULTI-TABLE INNER JOIN
-- ======================================================
SELECT D.Dept_name, E.Emp_fname, E.Emp_Position
FROM Employee E
INNER JOIN Dept D     ON E.Dept_id = D.Dept_id
INNER JOIN Project P  ON D.Dept_id = P.Dept_id
WHERE P.Proj_year = 2020;

-- ======================================================
-- 5️⃣  Display Emp_Position, Dept_name for which Project cost > 30000
--      → Use LEFT JOIN (include depts even if no project)
-- ======================================================
SELECT DISTINCT E.Emp_Position, D.Dept_name, P.Proj_cost
FROM Employee E
LEFT JOIN Dept D   ON E.Dept_id = D.Dept_id
LEFT JOIN Project P ON D.Dept_id = P.Dept_id
WHERE P.Proj_cost > 30000;

-- ======================================================
-- 6️⃣  Find names of all Projects that started in 2015
-- ======================================================
SELECT Proj_Name, Proj_year
FROM Project
WHERE Proj_year = 2015;

-- ======================================================
-- 7️⃣  List Dept_name having no_of_emp = 10
--      → Use GROUP BY + HAVING (aggregates)
-- ======================================================
SELECT D.Dept_name, COUNT(E.Emp_id) AS no_of_emp
FROM Dept D
LEFT JOIN Employee E ON D.Dept_id = E.Dept_id
GROUP BY D.Dept_name
HAVING no_of_emp = 10;

-- ======================================================
-- 8️⃣  Display total number of employees who joined any project before 2009
--      → Use RIGHT JOIN (to include all projects)
-- ======================================================
SELECT COUNT(DISTINCT E.Emp_id) AS Total_Employees
FROM Employee E
RIGHT JOIN Project P ON E.Dept_id = P.Dept_id
WHERE YEAR(E.Emp_JoinDate) < 2009;

-- ======================================================
-- 9️⃣  Create a view showing the Employee and Department details
-- ======================================================
CREATE VIEW emp_dept_view AS
SELECT E.Emp_id, E.Emp_fname, E.Emp_lname,
       E.Emp_Position, E.Emp_salary, E.Emp_JoinDate,
       D.Dept_name, D.location
FROM Employee E
INNER JOIN Dept D ON E.Dept_id = D.Dept_id;

-- ======================================================
-- 🔟  Perform Manipulation on simple view (Insert, Update, Delete, Drop)
-- ======================================================

-- INSERT into view
INSERT INTO emp_dept_view (Emp_id, Emp_fname, Emp_lname, Emp_Position,
                           Emp_salary, Emp_JoinDate, Dept_name, location)
VALUES (111, 'Riya', 'Sharma', 'Analyst', 55000, '2011-06-10', 'Finance', 'Delhi');

-- UPDATE through view
UPDATE emp_dept_view
SET Emp_salary = Emp_salary * 1.10
WHERE Emp_id = 111;

-- DELETE through view
DELETE FROM emp_dept_view
WHERE Emp_id = 111;

-- DROP view
DROP VIEW emp_dept_view;
