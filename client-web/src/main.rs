use actix_web::{web::Data, App, HttpServer};
use dotenv::dotenv;
use std::sync::Mutex;
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

    HttpServer::new(move || {
        App::new()
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
