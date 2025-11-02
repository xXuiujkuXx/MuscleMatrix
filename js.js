
let mainNav = document.getElementById('js-menu');
let navBarToggle = document.getElementById('js-nav-toggle');

navBarToggle.addEventListener("click", function(){
    mainNav.classList.toggle('active');
});

// ---------------- MQTT Setup ----------------
const broker = "wss://mqtt-dashboard.com:8884/mqtt"; // ใช้ WebSocket (wss)
const topic = "testtopic/Peemza";

// สร้าง client
const client = mqtt.connect(broker);

client.on("connect", () => {
  console.log("✅ Connected to MQTT broker");
});

// เมื่อมี error
client.on("error", (err) => {
  console.error("❌ MQTT Error:", err);
});

// ---------------- ส่ง mode ----------------
function sendMode(mode) {
  const payload = `mode=${mode}`;
  client.publish(topic, payload);
  console.log("📤 Sent:", payload);
}

// ---------------- การคลิก box ----------------
document.querySelector(".dumbell").addEventListener("click", () => {
  sendMode(0);
  window.location.href = "dumbell.html"; // เปลี่ยนหน้า
});

document.querySelector(".barbell").addEventListener("click", () => {
  sendMode(1);
  console.log("Switched to Barbell mode");
});




