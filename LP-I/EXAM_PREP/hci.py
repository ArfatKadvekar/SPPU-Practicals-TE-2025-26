import tkinter as tk
from tkinter import ttk, messagebox
from datetime import datetime

# ---------------- Functions ----------------

def show_page(page_name):
    """Switch between pages and update status bar"""
    for frame in pages.values():
        frame.pack_forget()
    pages[page_name].pack(fill="both", expand=True)
    status_bar.config(text=f"Current Page: {page_name.capitalize()}")

def register_student():
    """Validate and show confirmation for registration"""
    name = name_var.get().strip()
    roll = roll_var.get().strip()
    course = course_var.get()

    if not name:
        messagebox.showerror("Error", "Please enter your name.")
        name_entry.focus_set()
        return
    if not roll:
        messagebox.showerror("Error", "Please enter your Roll Number.")
        roll_entry.focus_set()
        return
    if course == "Select a course":
        messagebox.showerror("Error", "Please select a course.")
        return

    messagebox.showinfo("Success", f"Student {name} registered successfully on {datetime.now().strftime('%H:%M:%S')}!")

def submit_feedback():
    """Validate and store feedback"""
    rating = rating_var.get()
    comments = comment_box.get("1.0", "end-1c").strip()

    if not rating:
        messagebox.showerror("Error", "Please select a rating.")
        return
    if not comments:
        messagebox.showerror("Error", "Please write your feedback.")
        return

    messagebox.showinfo("Thank You", "Feedback submitted successfully!")
    clear_feedback()

def clear_feedback():
    rating_var.set("")
    comment_box.delete("1.0", "end")

# ---------------- UI Setup ----------------

root = tk.Tk()
root.title("Student Feedback System")
root.geometry("700x500")
root.config(bg="#f9f9f9")

# Sidebar for navigation
sidebar = tk.Frame(root, bg="#007acc", width=180)
sidebar.pack(side="left", fill="y")

tk.Label(sidebar, text="Navigation", bg="#007acc", fg="white",
         font=("Helvetica", 14, "bold")).pack(pady=15)

def nav_button(text, page):
    return tk.Button(sidebar, text=text, bg="#007acc", fg="white",
                     font=("Helvetica", 12), bd=0, width=15,
                     command=lambda: show_page(page))

nav_button("Register", "register").pack(pady=8)
nav_button("Feedback", "feedback").pack(pady=8)
nav_button("About", "about").pack(pady=8)

# ---------------- Pages ----------------
pages = {}

# -------- Register Page --------
register_page = tk.Frame(root, bg="#f9f9f9")
tk.Label(register_page, text="Student Registration",
         font=("Helvetica", 20, "bold"), bg="#f9f9f9", fg="#007acc").pack(pady=20)

form_frame = tk.Frame(register_page, bg="#f9f9f9")
form_frame.pack(pady=10)

tk.Label(form_frame, text="Name:*", font=("Helvetica", 12), bg="#f9f9f9").grid(row=0, column=0, sticky="e", padx=5, pady=5)
name_var = tk.StringVar()
name_entry = tk.Entry(form_frame, textvariable=name_var, width=30)
name_entry.grid(row=0, column=1, pady=5)

tk.Label(form_frame, text="Roll Number:*", font=("Helvetica", 12), bg="#f9f9f9").grid(row=1, column=0, sticky="e", padx=5, pady=5)
roll_var = tk.StringVar()
roll_entry = tk.Entry(form_frame, textvariable=roll_var, width=30)
roll_entry.grid(row=1, column=1, pady=5)

tk.Label(form_frame, text="Course:*", font=("Helvetica", 12), bg="#f9f9f9").grid(row=2, column=0, sticky="e", padx=5, pady=5)
course_var = tk.StringVar()
course_cb = ttk.Combobox(form_frame, textvariable=course_var, values=["HCI", "DBMS", "DS", "Software Engg"], state="readonly", width=27)
course_cb.set("Select a course")
course_cb.grid(row=2, column=1, pady=5)

tk.Button(register_page, text="Register", bg="#4CAF50", fg="white", width=15,
          font=("Helvetica", 12), command=register_student).pack(pady=20)

pages["register"] = register_page

# -------- Feedback Page --------
feedback_page = tk.Frame(root, bg="#f9f9f9")

tk.Label(feedback_page, text="Submit Feedback",
         font=("Helvetica", 20, "bold"), bg="#f9f9f9", fg="#007acc").pack(pady=20)

tk.Label(feedback_page, text="Rating (1 - Poor, 5 - Excellent):", font=("Helvetica", 12),
         bg="#f9f9f9").pack(pady=5)
rating_var = tk.StringVar()
rating_combo = ttk.Combobox(feedback_page, textvariable=rating_var, values=["1", "2", "3", "4", "5"], state="readonly", width=5)
rating_combo.pack(pady=5)

tk.Label(feedback_page, text="Comments:", font=("Helvetica", 12),
         bg="#f9f9f9").pack(pady=5)
comment_box = tk.Text(feedback_page, height=5, width=50)
comment_box.pack(pady=5)

tk.Button(feedback_page, text="Submit", bg="#4CAF50", fg="white", width=15,
          font=("Helvetica", 12), command=submit_feedback).pack(side="left", padx=50, pady=20)
tk.Button(feedback_page, text="Clear", bg="#f44336", fg="white", width=15,
          font=("Helvetica", 12), command=clear_feedback).pack(side="left", pady=20)

pages["feedback"] = feedback_page

# -------- About Page --------
about_page = tk.Frame(root, bg="#f9f9f9")

tk.Label(about_page, text="About the System",
         font=("Helvetica", 20, "bold"), bg="#f9f9f9", fg="#007acc").pack(pady=30)

info_text = (
    "Technologies Used:\n"
    "• Python (Tkinter for GUI design)\n"
    "• ttk Widgets for modern look\n"
    "• datetime module for real-time data\n\n"
    "Key Features:\n"
    "• Student registration and feedback pages\n"
    "• Input validation and user feedback messages\n"
    "• Simple, consistent, and user-friendly design\n\n"
    "HCI Principles Applied:\n"
    "• Usability – Easy navigation and layout\n"
    "• Feedback – Real-time popups and status bar\n"
    "• Consistency – Same color, font, and structure\n"
    "• Error Prevention – Validation for inputs"
)

tk.Label(about_page, text=info_text, font=("Helvetica", 12),
         bg="#f9f9f9", justify="left", wraplength=520).pack(padx=25, pady=10)

tk.Label(about_page, text="Developed by Tony Stark – HCI Project 2025",
         font=("Helvetica", 10, "italic"), fg="gray", bg="#f9f9f9").pack(pady=20)

pages["about"] = about_page

# ---------------- Status Bar ----------------
status_bar = tk.Label(root, text="Current Page: Register", bd=1, font=("Helvetica", 10), bg="#e6e6e6")
status_bar.pack(side="bottom", fill="x")

# ---------------- Start with Register ----------------
show_page("register")

root.mainloop()
