-- ======================================================
-- ASSIGNMENT NO 3  (BASED ON STUDENT SCHEMA)
-- ======================================================

-- SCHEMA:
-- Student (s_id, Drive_id, T_id, s_name, CGPA, s_branch, s_dob)
-- PlacementDrive (Drive_id, Pcompany_name, package, location)
-- Training (T_id, Tcompany_name, T_Fee, T_date)

-- ======================================================
-- 1️⃣  Find the Student details and Placement details
--      → Use NATURAL JOIN
-- ======================================================
SELECT *
FROM Student
NATURAL JOIN PlacementDrive;

-- ======================================================
-- 2️⃣  Find all the student details with company_name
--      who have conducted in same drive
--      → Use INNER JOIN
-- ======================================================
SELECT S.s_id, S.s_name, S.s_branch, P.Pcompany_name, P.location
FROM Student S
INNER JOIN PlacementDrive P ON S.Drive_id = P.Drive_id;

-- ======================================================
-- 3️⃣  List all the Student name and branch having package = 5 LPA
--      → Use INNER JOIN
-- ======================================================
SELECT S.s_name, S.s_branch, P.package
FROM Student S
INNER JOIN PlacementDrive P ON S.Drive_id = P.Drive_id
WHERE P.package = 500000;

-- ======================================================
-- 4️⃣  List all the student names, company_name having T_fee > 20000
--      → Use LEFT JOIN (show students even if training missing)
-- ======================================================
SELECT S.s_name, T.Tcompany_name, T.T_Fee
FROM Student S
LEFT JOIN Training T ON S.T_id = T.T_id
WHERE T.T_Fee > 20000;

-- ======================================================
-- 5️⃣  Display all training details attended by “Shantanu” in 2011
--      → Use INNER JOIN with YEAR() filter
-- ======================================================
SELECT T.*
FROM Student S
INNER JOIN Training T ON S.T_id = T.T_id
WHERE S.s_name = 'Shantanu'
AND YEAR(T.T_date) = 2011;

-- ======================================================
-- 6️⃣  List total number of companies who conduct training before 2015
--      → No join needed here
-- ======================================================
SELECT COUNT(DISTINCT Tcompany_name) AS Total_Companies
FROM Training
WHERE YEAR(T.T_date) < 2015;

-- ======================================================
-- 7️⃣  List student name with company ‘Microsoft’ and location ‘Thane’
--      → Use RIGHT JOIN (to include all drives even if no student)
-- ======================================================
SELECT S.s_name, P.Pcompany_name, P.location
FROM Student S
RIGHT JOIN PlacementDrive P ON S.Drive_id = P.Drive_id
WHERE P.Pcompany_name = 'Microsoft'
  AND P.location = 'Thane';

-- ======================================================
-- 8️⃣  Find names of all Students who joined ‘Microsoft’ training in 2015
--      → Use INNER JOIN
-- ======================================================
SELECT S.s_name, T.Tcompany_name, T.T_date
FROM Student S
INNER JOIN Training T ON S.T_id = T.T_id
WHERE T.Tcompany_name = 'Microsoft'
  AND YEAR(T.T_date) = 2015;

-- ======================================================
-- 9️⃣  Create a view showing the Student and Training details
-- ======================================================
CREATE VIEW student_training_view AS
SELECT S.s_id, S.s_name, S.s_branch, T.Tcompany_name, T.T_Fee, T.T_date
FROM Student S
INNER JOIN Training T ON S.T_id = T.T_id;

-- ======================================================
-- 🔟  Perform manipulation on the simple view
-- ======================================================

-- INSERT into view
INSERT INTO student_training_view (s_id, s_name, s_branch, Tcompany_name, T_Fee, T_date)
VALUES (201, 'Ananya', 'Computer', 'SkillEdge', 18000, '2011-01-01');

-- UPDATE in view
UPDATE student_training_view
SET T_Fee = 20000
WHERE s_id = 201;

-- DELETE from view
DELETE FROM student_training_view
WHERE s_id = 201;

-- DROP view
DROP VIEW student_training_view;
