use std::io;

fn main() {
    loop {
        println!("Enter a Fahrenheit temperature");

        let mut t = String::new();
        io::stdin().read_line(&mut t)
            .expect("Read failed");

        let t: f64 = match t.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };

        let t_c = f_to_c(t);
        println!("Temperature in Celsius is {}", t_c);
        break;
    }
}

fn f_to_c (t: f64) -> f64 {
    ((t - 32.0) * (5.0 / 9.0))
}
