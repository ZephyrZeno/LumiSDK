// build.rs — link against liblumiapp
fn main() {
    println!("cargo:rustc-link-lib=lumiapp");
    println!("cargo:rustc-link-search=native=../../liblumiapp/build");
}
