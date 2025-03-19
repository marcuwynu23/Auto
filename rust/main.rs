use serde::Deserialize;
use serde_yaml;
use std::fs::{File, OpenOptions};
use std::io::{Read, Write};
use std::process::{Command, Stdio};
use std::thread::sleep;
use std::time::Duration;
use std::env;
use std::path::PathBuf;

/// Define the structure for YAML parsing
#[derive(Debug, Deserialize)]
struct Task {
    name: String,
    id: String, // Unique task ID for execution
    command: String, // Multi-line commands will be stored here
}

#[derive(Debug, Deserialize)]
struct Config {
    r#type: String, // "sync" or "async"
    runs_on: String, // "batch", "bash", or "powershell"
    tasks: Vec<Task>,
}

/// Writes the command into a temporary script file and ensures unique execution
fn write_to_script_file(task_id: &str, command: &str, shell: &str) -> PathBuf {
    let temp_dir = env::temp_dir();
    let extension = match shell {
        "batch" => "bat",
        "bash" => "sh",
        "powershell" => "ps1",
        _ => "sh",
    };
    let script_filename = format!("devtask_exec_{}.{}", task_id.replace(" ", "_"), extension);
    let script_path = temp_dir.join(script_filename);

    let mut file = OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(&script_path)
        .expect("❌ Failed to create script file");

    if shell == "bash" {
        writeln!(file, "#!/bin/sh").expect("❌ Failed to write shebang to script file");
    }

    writeln!(file, "{}", command).expect("❌ Failed to write to script file");

    script_path
}

/// Determines the correct shell command to execute based on `runs-on`
fn get_shell_command(shell: &str, script_path: &PathBuf) -> Vec<String> {
    match shell {
        "batch" => vec!["cmd".to_string(), "/C".to_string(), script_path.to_str().unwrap().to_string()],
        "bash" => vec!["busybox".to_string(), "sh".to_string(), script_path.to_str().unwrap().to_string()],
        "powershell" => vec!["powershell".to_string(), "-ExecutionPolicy".to_string(), "Bypass".to_string(), "-File".to_string(), script_path.to_str().unwrap().to_string()],
        _ => vec!["busybox".to_string(), "sh".to_string(), script_path.to_str().unwrap().to_string()],
    }
}

/// Executes a command synchronously and waits for it to finish
fn execute_command_sync(task_id: &str, task_name: &str, command: &str, shell: &str) {
    let script_file = write_to_script_file(task_id, command, shell);
    let shell_command = get_shell_command(shell, &script_file);

    println!("\n🛠  Running Task: {}  ", task_name);
    println!("   ➜ Mode: sync  |  Shell: {}", shell);
    println!("--------------------------------------------------");

    let status = Command::new(&shell_command[0])
        .args(&shell_command[1..])
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status();

    match status {
        Ok(exit_status) if exit_status.success() => {
            println!("✅  Completed Sync Task: {}\n", task_id);
        }
        Ok(exit_status) => {
            eprintln!("❌  Task `{}` failed!", task_id);
            eprintln!("   ➜ Exit Code: {}", exit_status);
            println!("--------------------------------------------------\n");
        }
        Err(e) => {
            eprintln!("❌  Error running Sync Task `{}`: {}", task_id, e);
            println!("--------------------------------------------------\n");
        }
    }
}

/// Executes a command asynchronously in a new terminal
fn execute_command_async(task_id: &str, task_name: &str, command: &str, shell: &str) {
    let script_file = write_to_script_file(task_id, command, shell);
    let shell_command = get_shell_command(shell, &script_file);

    println!("\n🛠  Running Task: {}  ", task_name);
    println!("   ➜ Mode: async  |  Shell: {}", shell);
    println!("--------------------------------------------------");

    let result = Command::new("cmd")
        .args(&["/C", "start", "cmd", "/K"])
        .args(&shell_command)
        .spawn();

    if let Err(e) = result {
        eprintln!("❌  Failed to execute async task `{}`: {}", task_id, e);
    }

    sleep(Duration::from_millis(100));
}

/// Reads and executes tasks from the YAML file using the **global type**
fn execute_tasks(config: Config, target_task: Option<String>) {
    let is_async = config.r#type.to_lowercase() == "async";
    let shell = config.runs_on.to_lowercase();

    for task in &config.tasks {
        if let Some(ref task_id) = target_task {
            if &task.id != task_id {
                continue;
            }
        }

        if is_async {
            execute_command_async(&task.id, &task.name, &task.command, &shell);
        } else {
            execute_command_sync(&task.id, &task.name, &task.command, &shell);
        }

        if target_task.is_some() {
            break;
        }
    }

    if let Some(task_id) = target_task {
        if !config.tasks.iter().any(|task| task.id == task_id) {
            eprintln!("❌  Task ID `{}` not found!", task_id);
        }
    }
}

/// Loads the YAML config file
fn load_yaml_config(file_path: &str) -> Config {
    let mut file = File::open(file_path).expect("❌ Failed to open config file");
    let mut contents = String::new();
    file.read_to_string(&mut contents)
        .expect("❌ Failed to read config file");

    serde_yaml::from_str(&contents).expect("❌ Failed to parse YAML file")
}

/// Main function: Load YAML and execute tasks
fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() == 1 || (args.len() == 2 && args[1] == "--help") {
        show_help();
        std::process::exit(0);
    }

    let mut file_name = "tasks.yml".to_string();
    let mut target_task: Option<String> = None;

    if args.len() == 2 {
        target_task = Some(args[1].clone());
    } else if args.len() == 4 && args[1] == "-f" {
        file_name = args[2].clone();
        target_task = Some(args[3].clone());
    } else {
        eprintln!("❌ Invalid command format!");
        show_help();
        std::process::exit(1);
    }

    let config = load_yaml_config(&file_name);
    execute_tasks(config, target_task);
}

/// Displays a comprehensive help message
fn show_help() {
    println!("📌 DevTask - A simple automation tool ");
    println!("--------------------------------------------------");
    println!("🚀  USAGE:");
    println!("  devtask <task_id>         # Run a specific task from 'tasks.yml'");
    println!("  devtask -f <file.yml> <task_id>  # Run a specific task from a custom YAML file");
    println!("  devtask --help            # Show this help message");
    println!("--------------------------------------------------");
    println!("📜  TASKS.YML STRUCTURE:");
    println!("type: sync  # or 'async' (Controls task execution globally)");
    println!("runs_on: powershell  # Options: 'batch', 'bash', 'powershell'");
    println!("tasks:");
    println!("  - name: Print Hello World!");
    println!("    id: hello");
    println!("    command: write \"Hello World\"");
    println!("--------------------------------------------------");
    println!("💡  SHELL EXAMPLES:");
    println!("🟢  Batch (CMD)");
    println!("    runs_on: batch");
    println!("    command: echo Hello World");
    println!("🔵  Bash (Busybox)");
    println!("    runs_on: bash");
    println!("    command: echo \"Hello World\"");
    println!("🟣  PowerShell");
    println!("    runs_on: powershell");
    println!("    command: write \"Hello World\"");
    println!("--------------------------------------------------");
    println!("🌟  EXAMPLES:");
    println!("1️⃣  Run a task:");
    println!("   devtask hello");
    println!("2️⃣  Run a task from a custom YAML file:");
    println!("   devtask -f my_tasks.yml hello");
    println!("--------------------------------------------------");
    println!("📌  More features coming soon! 🚀");
}
