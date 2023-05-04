use dotenv::dotenv;
use std::{
    io::{BufRead, BufReader},
    str,
    time::Duration,
};
mod util;
use crate::util::camera;

fn main() {
    const BAUD_RATE: u32 = 115_200;
    let ports = serialport::available_ports().expect("No ports!");
    let mut found = false;
    let mut port = None;
    let camera = camera::Camera::new();

    dotenv().ok();

    for p in ports {
        println!("{}", p.port_name);
        if let true = p.port_name.contains("ACM") {
            println!("USB bus found on {}", p.port_name);
            port = Some(
                serialport::new(&p.port_name, BAUD_RATE)
                    .timeout(Duration::from_millis(20))
                    .open()
                    .unwrap(),
            );
            println!("Port opened on {}", p.port_name);
            found = true;
        }
    }
    assert!(found, "No USB bus found!");

    if let Some(serial) = port {
        let mut reader = BufReader::new(serial);
        loop {
            let mut buf = String::new();
            match reader.read_line(&mut buf) {
                Ok(_t) => {
                    if let Some(angle) = parse_angle(buf.as_bytes()) {
                        camera.pan(angle);
                    } else {
                        println!("Invalid angle");
                    }
                }
                Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => (),
                Err(e) => eprintln!("{e:?}"),
            }
        }
    }
}

fn parse_angle(buf: &[u8]) -> Option<i16> {
    let binding = buf.to_vec();
    let deg: i16;

    let string = str::from_utf8(&binding)
        .unwrap_or("Invalid UTF-8 sequence")
        .trim_matches(|c| c == '\r' || c == '\n');

    if let Ok(n) = string.parse::<f32>() {
        deg = n.to_degrees().round() as i16;
        println!("Angle: {deg}");
    } else {
        println!("{string}");
        return None;
    }
    let angle = -((deg + 600) % 360 - 180);
    Some(angle)
}
