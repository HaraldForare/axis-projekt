use crate::util::camera::Camera;
use actix_web::{get, post, web, HttpResponse, Responder};
use std::sync::Mutex;

#[post("/vapix/pan/{deg}")]
async fn pan(
    deg: web::Path<i16>,
    camera: web::Data<Mutex<Camera>>,
    current_pos: web::Data<Mutex<i16>>,
) -> impl Responder {
    //update current position (rly ugly)
    if let Ok(x) = current_pos.try_lock().as_mut() {
        **x = ((**x + deg.into_inner() + 540) % 360 + 360) % 360 - 180;
    }

    //then pan the camera by deg degrees
    if let Ok(client) = camera.try_lock() {
        if let Ok(current) = current_pos.try_lock() {
            client.pan(*current).await;
            return HttpResponse::Ok();
        }
    }
    HttpResponse::InternalServerError()
}

#[get("/vapix/position")]
async fn get_pos(current_pos: web::Data<Mutex<i16>>) -> impl Responder {
    if let Ok(x) = current_pos.try_lock() {
        return HttpResponse::Ok().json(*x);
    }
    HttpResponse::InternalServerError().json("")
}
