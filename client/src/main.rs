use actix_web::{web, App, HttpServer};
use dotenv::dotenv;
use std::sync::Mutex;

mod routes;
mod util;

#[actix_web::main]
async fn main() {
    dotenv().ok();
    let camera = util::camera::Camera::new();
    camera.pan(0).await;
    let camera_mut = web::Data::new(Mutex::new(camera));
    let current_pos: web::Data<Mutex<i16>> = web::Data::new(Mutex::new(0));

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
