use serialport;
use std::{
    io::{self, BufRead, BufReader},
    str,
    time::Duration,
};

fn main() {
    const BAUD_RATE: u32 = 115200;
    let ports = serialport::available_ports().expect("No ports!");
    let mut found = false;
    let mut port = None;

    for p in ports {
        println!("{}", p.port_name);
        if let true = p.port_name.contains("ACM") {
            println!("USB bus found on {}", p.port_name);
            port = Some(
                serialport::new(&p.port_name, BAUD_RATE)
                    .timeout(Duration::from_millis(50))
                    .open()
                    .unwrap(),
            );
            println!("Port opened on {}", p.port_name);
            found = true;
        }
    }
    if !found {
        panic!("No USB bus found!")
    }

    if let Some(serial) = port {
        let mut reader = BufReader::new(serial);
        loop {
            let mut buf = String::new();
            match reader.read_line(&mut buf) {
                Ok(_t) => {
                    test(buf.as_bytes());
                }
                Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => (),
                Err(e) => eprintln!("{:?}", e),
            }
        }
    }
}

fn test(buf: &[u8]) {
    let binding = buf.to_vec();
    let string = str::from_utf8(&binding)
        .unwrap_or("0")
        .trim_matches(|c| c == '\r' || c == '\n');
    println!("string: {}", &string);
    let angle = string.parse().unwrap_or(0.0_f32).to_degrees().round() as i32;
    std::thread::sleep(std::time::Duration::from_millis(100));
    println!("angle: {} \n", &angle);
}
