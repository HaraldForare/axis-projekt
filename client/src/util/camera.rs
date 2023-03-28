use std::env;

impl Camera {
    pub fn new() -> Self {
        Self {
            client: reqwest::Client::new(),
        }
    }

    pub async fn pan(&self, deg: i16) -> bool {
        if deg.abs() > 180 {
            return false;
        }

        let username = env::var("CAMERA_USER").unwrap_or(String::from("user"));
        let password = env::var("CAMERA_PASS").unwrap_or_default();

        self.client
            //Why tf is this a GET rq?
            .get(format!(
                "http://{}/axis-cgi/com/ptz.cgi?pan={deg}",
                env::var("CAMERA_IP").unwrap_or(String::from("127.0.0.1"))
            ))
            .basic_auth(username, Some(password))
            .send()
            .await
            .unwrap();
        true
    }
}

#[derive(Clone)]
pub struct Camera {
    pub client: reqwest::Client,
}
