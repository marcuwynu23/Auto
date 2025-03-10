use std::fs::File;
use std::io::{self, BufRead};
use std::process::Command;
use std::thread::sleep;
use std::time::Duration;
use std::sync::Mutex;
use std::sync::Arc;
use regex::Regex;

fn remove_quotes(s: &str) -> String {
    let re = Regex::new(r#"^["'](.*)["']$"#).unwrap();
    if let Some(caps) = re.captures(s) {
        return caps.get(1).map_or(s.to_string(), |m| m.as_str().to_string());
    }
    s.to_string()
}

fn split_command(command: &str) -> Vec<String> {
    let mut args = Vec::new();
    let mut current_arg = String::new();
    let mut in_quotes = false;

    for c in command.chars() {
        if c == '"' || c == '\'' {
            in_quotes = !in_quotes;
        } else if c == ' ' && !in_quotes {
            if !current_arg.is_empty() {
                args.push(remove_quotes(&current_arg));
                current_arg.clear();
            }
        } else {
            current_arg.push(c);
        }
    }
    if !current_arg.is_empty() {
        args.push(remove_quotes(&current_arg));
    }
    args
}

fn execute_command_sync(command: &str) {
    let clean_command = split_command(command).join(" ");
    let output = Command::new("cmd")
        .arg("/C")
        .arg(format!("start cmd /K {}", clean_command))
        .output();
    
    match output {
        Ok(output) => {
            if !output.stdout.is_empty() {
                println!("Output: {}", String::from_utf8_lossy(&output.stdout));
            }
            if !output.stderr.is_empty() {
                eprintln!("Error: {}", String::from_utf8_lossy(&output.stderr));
            }
        }
        Err(e) => {
            eprintln!("Failed to execute command: {}", e);
        }
    }
}

fn execute_command_async(command: String) {
    let clean_command = split_command(&command).join(" ");
    if let Err(e) = Command::new("cmd")
        .arg("/C")
        .arg(format!("start cmd /K {}", clean_command))
        .spawn()
    {
        eprintln!("Failed to execute command asynchronously: {}", e);
    }
    sleep(Duration::from_millis(100));
}

fn process_blocks(file_path: &str, target_block: &str, async_mode: bool) {
    if let Ok(file) = File::open(file_path) {
        let reader = io::BufReader::new(file);
        let execution_type = Arc::new(Mutex::new(0));
        let mut current_block = String::new();
        let mut block_found = false;

        for line in reader.lines() {
            if let Ok(mut line) = line {
                line = line.trim().to_string();
                if line.is_empty() { continue; }
                
                if line.starts_with("::ASYNC") {
                    println!("Running in ASYNC mode");
                    continue;
                } else if line.starts_with("::SYNC") {
                    println!("Running in SYNC mode");
                    continue;
                }
                
                if line.ends_with('{') {
                    current_block = line[..line.len() - 1].trim().to_string();
                    if current_block == target_block || current_block.starts_with('.') {
                        block_found = true;
                    }
                } else if line == "}" {
                    current_block.clear();
                } else if !current_block.is_empty() && (current_block == target_block || current_block.starts_with('.')) {
                    let command = line[1..].trim().to_string();
                    if async_mode {
                        println!("Executing Async: {}", command);
                        execute_command_async(command);
                    } else {
                        println!("Executing Sync: {}", command);
                        execute_command_sync(&command);
                    }
                }
            }
        }

        if !block_found {
            println!("Block '{}' not found in the script.", target_block);
        }
    } else {
        eprintln!("Failed to open file: {}", file_path);
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut file_name = ".autofile".to_string();
    let mut target_block = String::new();
    let mut async_mode = false;

    if args.len() == 2 {
        target_block = args[1].clone();
    } else if args.len() == 4 && args[1] == "-f" {
        file_name = args[2].clone();
        target_block = args[3].clone();
        
        if !file_name.starts_with('.') {
            eprintln!("Error: The file must be a hidden file (starting with '.').");
            std::process::exit(1);
        }
    } else {
        eprintln!("Usage: {} [-f <file_path>] <block_name", args[0]);
        std::process::exit(1);
    }
    
    if let Ok(file) = File::open(&file_name) {
        let reader = io::BufReader::new(file);
        for line in reader.lines() {
            if let Ok(line) = line {
                if line.trim().starts_with("::ASYNC") {
                    async_mode = true;
                } else if line.trim().starts_with("::SYNC") {
                    async_mode = false;
                }
            }
        }
    }
    
    process_blocks(&file_name, &target_block, async_mode);
}