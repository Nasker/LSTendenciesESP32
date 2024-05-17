/* scripts.js */
document.addEventListener('DOMContentLoaded', () => {
    const joystick = document.getElementById('joystick');
    const knob = document.getElementById('knob');
    let offsetX, offsetY, centerX, centerY;
  
    knob.addEventListener('mousedown', startDrag);
    knob.addEventListener('touchstart', startDrag);
  
    function startDrag(event) {
      event.preventDefault();
      if (event.type === 'mousedown') {
        document.addEventListener('mousemove', drag);
        document.addEventListener('mouseup', stopDrag);
      } else {
        document.addEventListener('touchmove', drag);
        document.addEventListener('touchend', stopDrag);
      }
  
      centerX = joystick.offsetWidth / 2;
      centerY = joystick.offsetHeight / 2;
  
      if (event.type === 'mousedown') {
        offsetX = event.clientX - knob.getBoundingClientRect().left;
        offsetY = event.clientY - knob.getBoundingClientRect().top;
      } else {
        offsetX = event.touches[0].clientX - knob.getBoundingClientRect().left;
        offsetY = event.touches[0].clientY - knob.getBoundingClientRect().top;
      }
    }
  
    function drag(event) {
      event.preventDefault();
  
      let clientX, clientY;
      if (event.type === 'mousemove') {
        clientX = event.clientX;
        clientY = event.clientY;
      } else {
        clientX = event.touches[0].clientX;
        clientY = event.touches[0].clientY;
      }
  
      let x = clientX - joystick.offsetLeft - offsetX;
      let y = clientY - joystick.offsetTop - offsetY;
  
      const maxRadius = joystick.offsetWidth / 2 - knob.offsetWidth / 2;
  
      const distance = Math.sqrt(x * x + y * y);
      if (distance > maxRadius) {
        x = (x / distance) * maxRadius;
        y = (y / distance) * maxRadius;
      }
  
      knob.style.left = `${x + centerX}px`;
      knob.style.top = `${y + centerY}px`;
  
      sendCoordinatesToESP32(x / maxRadius, y / maxRadius);
    }
  
    function stopDrag() {
      knob.style.left = `50%`;
      knob.style.top = `50%`;
  
      document.removeEventListener('mousemove', drag);
      document.removeEventListener('mouseup', stopDrag);
      document.removeEventListener('touchmove', drag);
      document.removeEventListener('touchend', stopDrag);
  
      sendCoordinatesToESP32(0, 0); // Send centered coordinates
    }
  
    function sendCoordinatesToESP32(x, y) {
      const url = `/send?x=${x}&y=${y}`;
      fetch(url).catch(error => console.error('Error:', error));
    }
  });
  