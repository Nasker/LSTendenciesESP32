document.addEventListener('DOMContentLoaded', () => {
  const progressElement = document.querySelector('.progress');
  const cursorElement = document.querySelector('.cursor');
  const progressPercentageElement = document.getElementById('progress-percentage');
  let cursorImages = ['cursor1.png', 'cursor2.png']; // Add more images as needed
  let currentImageIndex = 0;

  // Function to update progress
  function updateProgress(progress) {
    progressElement.style.width = progress + '%';
    progressPercentageElement.textContent = progress + '%';
  }

  // Fetch initial progress value from the server
  fetch('/get-progress')
    .then(response => response.text())
    .then(progress => {
      console.log('Initial progress:', progress);
      updateProgress(progress);
    });

  /*// Function to animate cursor
  function animateCursor() {
    cursorElement.style.backgroundImage = `url(${cursorImages[currentImageIndex]})`;
    currentImageIndex = (currentImageIndex + 1) % cursorImages.length;
  }

  // Set interval to change cursor image every second
  setInterval(animateCursor, 1000);
*/
  // Example function to be called when progress needs to be updated
  function setNewProgress(newProgress) {
    fetch(`/update-progress?value=${newProgress}`)
      .then(response => response.text())
      .then(message => {
        console.log(message);
        updateProgress(newProgress);
      });
  }

  // Example usage: Call setNewProgress with new value when needed
  // setNewProgress(80); // This would set progress to 80%
});
