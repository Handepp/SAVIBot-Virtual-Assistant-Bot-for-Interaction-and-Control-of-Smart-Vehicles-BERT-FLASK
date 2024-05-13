// teks
const msgerForm = get(".savi-input-div");
const msgerInput = get(".savi-input-message");
msgerForm.click()
// chatroom
const msgerChat = get(".savi-chat");
const BOT_IMG = "../static/img/Logo/Logo2.png";
const BOT_NAME = "SAVIBot";

// Ketika submit tombol
msgerForm.addEventListener("submit", event => {
event.preventDefault();
const msgText = msgerInput.value;
if (!msgText) return;
msgerInput.value = "";
botResponse(msgText);
});

tts("Halo! SAVI disini, Ada yang bisa SAVI bantu?");

// Menambahkan pesan respon
function appendMessage(name, img, side, text) {
    const msgHTML = `
    <div class="msg ${side}-msg">
      <div class="savi-img" id="openModalBtn" style="background-image: url(${img})"></div>
      <div id="myModal" class="modal">
        <div class="modal-content">
          <span class="close" id="closeModalBtn">&times;</span>
          <h3>Apakah ingin pindah ke mode kendali penuh dengan suara?</h3>
          <button id="confirmBtn2">Ya</button>
          <button id="cancelBtn2">Tidak</button>
        </div>
      </div>
      <div class="savi-bubble">
        <div class="savi-info">
          <div class="savi-info-name">${name}</div>
          <div class="savi-info-time">${formatDate(new Date())}</div>
        </div>
      <div class="savi-text">${text}</div> 
      </div>
    </div>
`;
    msgerChat.innerHTML=msgHTML
}



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
        iconUrl: '../static/img/Logo/Logo2.png',
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

// Memberikan respon
function botResponse(rawText) {
    // Bot Response
    $.get("/kendali/get", { prediction_input_kendali: rawText }).done(function (msg) {
      console.log("Input kendali =",rawText);
      console.log("Respon SAVI kendali =", msg);
      fetch('/tag').then(response => response.json()).then(data => {
        var label = data.response_tag;
        if (label == "Hidup" || label == "Mati" || label == "Maju" || label == "Mundur" || label == "Stop" || label == "Speed" || label == "Kanan" || label == "Kiri" || label == "Putar" ) {
          const msgText = msg;
          console.log(label)
          appendMessage(BOT_NAME, BOT_IMG, "left", msgText);
          tts(msg);
        }
        else{
          console.log("Tidak dapat memberikan respon umum pada mode ini")
        }
      });
    });

    
  }

/*document.getElementById("message").addEventListener("keyup", function (event) {
  if (event.keyCode === 13) {
    event.preventDefault();
    send();
  }
});*/

//utils
function get(selector, root = document) {
  return root.querySelector(selector);
}

// Format waktu 
function formatDate(date) {
  const h = "0" + date.getHours();
  const m = "0" + date.getMinutes();
  return `${h.slice(-2)}:${m.slice(-2)}`;
}

document.getElementById("time").textContent=formatDate(new Date())


const msgervoice = get("voicesavi");
const voiceChat = get("output_text")

const button = document.getElementById("voicesavi")
const message = document.getElementById("message")
var pulse = document.getElementById("pulse")
var micon = document.getElementById("micc")
var teks = document.getElementById("output_text")
var audioon = new Audio("../static/music/active_voice.mp3");
var audiooff = new Audio("../static/music/deactive_voice.mp3");

function stt() {
  window.SpeechRecognition = window.SpeechRecognition || window.webkitSpeechRecognition;
  if (('SpeechRecognition' in window || 'webkitSpeechRecognition' in window)) {
    let speech = {
      enabled: true,
      listening: true,
      recognition: new window.SpeechRecognition(),
      text: ''
    }
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
        //result.innerText = speech.text;
        //botResponse(speech.text);
        botResponse(speech.text)
    });
    speech.recognition.start();
    speech.recognition.addEventListener('end', () => {
      console.log("Gagal membaca suara, mengulang lagi")
      speech.recognition.start();
    });
    if (speech.listening) {
      voicesavi.classList.add('listening');
      button.style.backgroundColor='green';
      audioon.play();
      pulse.style.borderColor='green';
      micon.src="../static/img/micon.png";
      console.log("Input suara = ",speech.text)
    }
  }
}
stt();


function handleKeyPress(event) {
  if (event.altKey && event.key === 'v') {
    button.click()
    console.log('Fitur suara diaktifkan melalui shortcut');
  }

  if (event.altKey && event.key === 'c') {
    message.focus()
    console.log('Fitur ketik diaktifkan melalui shortcut');
  }
}

// Menambahkan event listener untuk menanggapi peristiwa tombol
document.addEventListener('keydown', handleKeyPress);

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

function changeBackgroundColor(status) {
  const body = document.body;
  if (status === 'SAVI ON') {
    Status_green.style.opacity = '100%'; // Ubah warna menjadi hijau]
    Status_red.style.opacity = '25%';
  } else if (status === 'SAVI OFF') {
    Status_red.style.opacity = '100%';; // Ubah warna menjadi merah
    Status_green.style.opacity = '25%';
  }
}

// Fungsi untuk memuat status relay dari server Flask
function getStatus() {
  fetch('/status')
      .then(response => response.json())
      .then(data => {
          changeBackgroundColor(data.status);
      })
      .catch(error => console.error('Error:', error));
}

// Memuat status relay saat halaman dimuat
document.addEventListener('DOMContentLoaded', getStatus);


// Memuat status relay secara berkala (misalnya, setiap 2 detik)
//setInterval(getStatus, 2000);

function mode(){
  const openModalBtn = document.getElementById('openModalBtn');
  const closeModalBtn = document.getElementById('closeModalBtn');
  const confirmBtn = document.getElementById('confirmBtn');
  const cancelBtn = document.getElementById('cancelBtn');
  const modal = document.getElementById('myModal');

  openModalBtn.addEventListener('click', function () {
    modal.style.display = 'block';
  });

  closeModalBtn.addEventListener('click', function () {
    modal.style.display = 'none';
  });

  confirmBtn.addEventListener('click', function () {

    modal.style.display = 'none';
    window.location.href = '/';
  });

  cancelBtn.addEventListener('click', function () {

    modal.style.display = 'none';
  });
}

document.addEventListener('DOMContentLoaded', mode);
//setInterval(mode, 2000);

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
}


document.addEventListener('DOMContentLoaded', getdiagnosa);


// Memuat status relay secara berkala (misalnya, setiap 2 detik)
setInterval(getdiagnosa, 2000);*/

