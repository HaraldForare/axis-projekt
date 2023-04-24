use dotenv::dotenv;
use serialport;
use std::{
    io::{BufRead, BufReader},
    str,
    time::Duration,
};

use crate::util::camera;

mod util;
fn main() {
    dotenv().ok();
    const BAUD_RATE: u32 = 115200;
    let ports = serialport::available_ports().expect("No ports!");
    let mut found = false;
    let mut port = None;
    let camera = camera::Camera::new();

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
                    camera.pan(parse_angle(buf.as_bytes()));
                }
                Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => (),
                Err(e) => eprintln!("{:?}", e),
            }
        }
    }
}

fn parse_angle(buf: &[u8]) -> i16 {
    let binding = buf.to_vec();
    let string = str::from_utf8(&binding)
        .unwrap_or("0")
        .trim_matches(|c| c == '\r' || c == '\n');
    println!("string: {}", &string);
    let angle = string.parse().unwrap_or(0.0_f32).to_degrees().round() as i16;
    std::thread::sleep(std::time::Duration::from_millis(100));
    println!("angle: {} \n", &angle);
    angle
}
