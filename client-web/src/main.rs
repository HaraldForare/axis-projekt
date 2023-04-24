use actix_web::{web::Data, App, HttpServer};
use dotenv::dotenv;
use std::{
    sync::{Arc, Mutex},
    thread,
};
//use util::camera::return_to_zero;

mod routes;
mod util;

#[actix_web::main]
async fn main() {
    dotenv().ok();
    let camera = util::camera::Camera::new();
    camera.pan(0).await;
    let camera_mut = Data::new(Mutex::new(camera));
    let current_pos: Data<Mutex<i16>> = Data::new(Mutex::new(0));
    let t: Data<Mutex<i128>> = Data::new(Mutex::new(0));
    //let t_arg = t.clone();

    // thread::spawn(move || return_to_zero(t_arg));

    HttpServer::new(move || {
        App::new()
            //.app_data(web::Data::new(t).clone())
            .app_data(camera_mut.clone())
            .app_data(current_pos.clone())
            .service(routes::api::vapix::pan)
            .service(routes::api::vapix::get_pos)
    })
    .bind(("0.0.0.0", 8080))
    .unwrap()
    .run()
    .await
    .unwrap();
}
