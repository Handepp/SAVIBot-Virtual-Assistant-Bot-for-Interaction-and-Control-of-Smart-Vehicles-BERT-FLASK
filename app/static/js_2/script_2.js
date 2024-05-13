document.getElementById("chatbot_toggle").onclick = function () {
  if (document.getElementById("chatbot").classList.contains("collapsed")) {
    document.getElementById("chatbot").classList.remove("collapsed")
    document.getElementById("chatbot_toggle").children[0].style.display = "none"
    document.getElementById("chatbot_toggle").children[1].style.display = ""
    document.getElementById("chatbot_toggle").style.width = "48px"
    document.getElementById("close").style.display = "block"
  }
  else {
    document.getElementById("chatbot").classList.add("collapsed")
    document.getElementById("chatbot_toggle").children[0].style.display = ""
    document.getElementById("chatbot_toggle").children[1].style.display = "none"
    document.getElementById("chatbot_toggle").style.width = "125px"
  }
}

// teks
const msgerForm = get(".input-div");
const msgerInput = get(".input-message");

// chatroom
const msgerChat = get(".msger-chat");
const BOT_IMG = "../static/img_2/Logo/Logo2.png";
const PERSON_IMG = "../static/img_2/man.png";
const BOT_NAME = "SAVIBot";
const PERSON_NAME = "Kamu";

// sound on/off
var soundon = document.getElementById("soundon");
var soundoff = document.getElementById("soundoff");

// ketika submit input
msgerForm.addEventListener("submit", event => {
  event.preventDefault();
  const msgText = msgerInput.value;
  if (!msgText) return;
  appendMessage(PERSON_NAME, PERSON_IMG, "right", msgText);
  msgerInput.value = "";
  botResponse(msgText);
});

// Fungsi append pesan ke chatroom
function appendMessage(name, img, side, text) {
  //   Simple solution for small apps
  const msgHTML = `
<div class="msg ${side}-msg">
  <div class="msg-img" style="background-image: url(${img})"></div>
  <div class="msg-bubble">
    <div class="msg-info">
      <div class="msg-info-name">${name}</div>
      <div class="msg-info-time">${formatDate(new Date())}</div>
    </div>
    <div class="msg-text">${text}</div>
  </div>
</div>
`;
  //msgerChat.innerHTML=msgHTML
  msgerChat.insertAdjacentHTML("beforeend", msgHTML);
  msgerChat.scrollTop +=  500;
}

// Fungsi append lokasi ke chatroom
function appendMap(name, img, side) {
  //   Simple solution for small apps
  const msgHTML = `
<div class="msg ${side}-msg">
  <div class="msg-img" style="background-image: url(${img})"></div>
  <div class="msg-bubble">
    <div class="msg-info">
      <div class="msg-info-name">${name}</div>
      <div class="msg-info-time">${formatDate(new Date())}</div>
    </div>
    <div id="map"></div>
  </div>
</div>
`;
  //msgerChat.innerHTML=msgHTML
  msgerChat.insertAdjacentHTML("beforeend", msgHTML);
  savimap();
  msgerChat.scrollTop +=  500;
}

// Fungsi append foto ke chatroom
function appendphoto(name, img, side,count) {
  //   Simple solution for small apps
  
  const msgHTML = `
<div class="msg ${side}-msg">
  <div class="msg-img" style="background-image: url(${img})"></div>
  <div id="bubble" class="balon">
    <div class="msg-info">
      <div class="msg-info-name">${name}</div>
      <div class="msg-info-time">${formatDate(new Date())}</div>
    </div>
    <img id="video_feed" src="../static/screenshot_2/img${count}.jpg">
  </div>
</div>
`;
  //msgerChat.innerHTML=msgHTML
  msgerChat.insertAdjacentHTML("beforeend", msgHTML);
  //document.getElementById("video_feed").hidden = false;
  msgerChat.scrollTop +=  500;
}

// buat API map leaflet.js
function savimap(){
  fetch('/peta')  // Mengambil data dari rute Python
  .then(response => response.json())  // Menguraikan respons sebagai JSON
  .then(data => {
      // Mengakses dua variabel dari objek JSON
      let lat = data.lat;
      let long = data.longi;

      // Melakukan sesuatu dengan dua variabel tersebut
      console.log('Latitude:', lat);
      console.log('Longitude:', long);

      var iconsavi = L.icon({
        iconUrl: '../static/img_2/Logo/Logo2.png',
        iconSize:     [15, 25],
      });
    
      var map = L.map('map').setView([lat, long], 18);
      var marker = L.marker([lat, long], {icon: iconsavi}).addTo(map)
      .bindPopup('SAVI Disini!!');;
    
      L.tileLayer('https://tile.openstreetmap.org/{z}/{x}/{y}.png', {
          maxZoom: 19,
          attribution: '© OpenStreetMap'
      }).addTo(map);
    
      map.on('click', function(e) {
        var popLocation= e.latlng;
        console.log("Lat, Lon : " + e.latlng.lat + ", " + e.latlng.lng) 
        var popup = L.popup()
        .setLatLng(popLocation)
        .setContent("Lat : " + e.latlng.lat + "<br>" + "Lon : " + e.latlng.lng)
        .openOn(map);        
    });
  })
  .catch(error => console.error('Terjadi kesalahan:', error));
}

// Buat respon bot berdasarkan input
function botResponse(rawText) {
  
  $.get("/get", { prediction_input: rawText }).done(function (msg) {
    console.log(rawText);
    console.log(msg);
    const msgText = msg;
    appendMessage(BOT_NAME, BOT_IMG, "left", msgText);

    fetch('/tag').then(response => response.json()).then(data => {
      var label = data.response_tag;
      console.log(label)
      if (label == "Lokasi" ) {
        map.remove();
        appendMap(BOT_NAME, BOT_IMG, "left");
      }
      else if (label == "Photo"){
        //document.getElementById("video_feed").hidden = true;
        fetch('/image').then(response => response.json()).then(data => {
          var gambar = data.k2;
          gambar = gambar - 1
          appendphoto(BOT_NAME, BOT_IMG, "left",gambar);
        });
        //localStorage.setItem('i', i);
      }
    });
    

    if (soundon.style.display === "block") { //sound mati
      console.log("TTS Mati")
      tts(msg)
    } else { // sound on
      console.log("TTS Aktif")
    }
  });
  
}

document.getElementById("message").addEventListener("keyup", function (event) {
  if (event.keyCode === 13) {
    event.preventDefault();
  }
});

//utils
function get(selector, root = document) {
  return root.querySelector(selector);
}

function formatDate(date) {
  const h = "0" + date.getHours();
  const m = "0" + date.getMinutes();
  return `${h.slice(-2)}:${m.slice(-2)}`;
}

// Speech recognition WEB speech API
window.SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
let speech = {
  enabled: true,
  listening: false,
  recognition: new window.SpeechRecognition(),
  text: ''
}

var audioon = new Audio("../static/music_2/active_voice.mp3");
var audiooff = new Audio("../static/music_2/deactive_voice.mp3");

function init() {
  if (('SpeechRecognition' in window || 'webkitSpeechRecognition' in window)) {

    speech.recognition.continuous = true;
    speech.recognition.interimResults = false;
    speech.recognition.lang = 'id';
    speech.recognition.addEventListener('result', (event) => {
      const audio = event.results[event.results.length - 1];
      speech.text = audio[0].transcript;
      const tag = document.activeElement.nodeName;
      /*if (tag === 'INPUT' || tag === 'TEXTAREA') {
        if (audio.isFinal) {
          document.activeElement.value += speech.text;
        }
      }*/
      appendMessage(PERSON_NAME, PERSON_IMG, "right", speech.text);
      botResponse(speech.text)
      console.log(speech.text)
      voicetekan.click()
    });

    voicetekan.addEventListener('click', () => {
      speech.listening = !speech.listening;
      if (speech.listening) {
        voicetekan.classList.add('listening');
        voicetekan.style.backgroundColor='green';
        pulse_2.style.borderColor='green'
        audioon.play();
        speech.recognition.start();
      }
      else  {
        voicetekan.classList.remove('listening');
        voicetekan.style.backgroundColor='red';
        pulse_2.style.borderColor='red'
        audiooff.play();
        speech.recognition.stop();
      }
    })
  }
}
init();

// Buton aktivasi input suara
function voiceon() {
  var x = document.getElementById("voicemode");
  if (x.style.display === "none") {
    voicetekan.style.backgroundColor='red';
    pulse_2.style.borderColor='red'
    voicebutton.style.display='none'
    close2.style.display='block'
    x.style.display = "block";
  } else {
    voicebutton.style.display='block'
    close2.style.display='none'
    x.style.display = "none";
    speech.recognition.stop();
  }
}
voiceon()

// Text to speech Web Speech API
function tts(text){
  if ('speechSynthesis' in window || 'SpeechSynthesisUtterance' in window) {
    var synthesis = window.speechSynthesis;
  
    // Get the first `en` language voice in the list
    var voice = synthesis.getVoices().filter(function (voice) {
      return voice.lang === 'id';
    })[0];
  
    // Create an utterance object
    var utterance = new SpeechSynthesisUtterance(text);
  
    // Set utterance properties
    utterance.voice = voice;
    utterance.pitch = 1.5;
    utterance.rate = 1;
    utterance.volume = 1;
    utterance.lang = "id-ID";
  
    // Speak the utterance
    synthesis.speak(utterance);
  }
  else {
    console.log('Text-to-speech not supported.');
  }
}

// Button tts on/off
function ttson() {
  if (soundon.style.display === "block") { //sound mati
      console.log("TTS Mati")
      soundon.style.display = "none";
      soundoff.style.display = "block";
      audiooff.play();
  } else { // sound on
      console.log("TTS Aktif")
      soundon.style.display = "block";
      soundoff.style.display = "none";
      audioon.play();
  }
}

/*function getdiagnosa() {
  fetch('/diagnosa')
      .then(response => response.json())
      .then(data => {
          console.log(data)
          if (data.diagnosa === 's' || data.diagnosa === 'd' || data.diagnosa === 't'){
            appendMessage(BOT_NAME, BOT_IMG, "left", data.respons_diagnosa);
            tts(data.respons_diagnosa);
          }
      })
      .catch(error => console.error('Error:', error));
}*/

// Memuat status relay saat halaman dimuat
//document.addEventListener('DOMContentLoaded', getdiagnosa);


// Memuat status relay secara berkala (misalnya, setiap 2 detik)
//setInterval(getdiagnosa, 100);


/*setInterval(function() {
  document.getElementById('video_feed').src = '/get_camera_feed';
}, 1000);  // Gambar diperbarui setiap 1 detik

/*let video = document.querySelector("#video");

async function videoon(){
  let stream = await navigator.mediaDevices.getUserMedia({ video: true, audio: false });
  video.srcObject = stream;
}

videoon()

function saviphoto(){
  let canvas = document.querySelector("#canvas");
  canvas.getContext('2d').drawImage(video, 0, 0, canvas.width, canvas.height);
  
}*/





