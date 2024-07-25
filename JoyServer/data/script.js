document.addEventListener('DOMContentLoaded', () => {
  const progressElement = document.querySelector('.progress');
  const cursorElement = document.querySelector('.cursor');
  let currentProgress = 1; // Initial progress value
  let cursorImages = ['cursor1.png', 'cursor2.png']; // Add more images as needed
  let currentImageIndex = 0;

  // Function to update progress
  function updateProgress(progress) {
    progressElement.style.width = progress + '%';
  }

  // Function to animate cursor
  function animateCursor() {
    cursorElement.style.backgroundImage = `url(${cursorImages[currentImageIndex]})`;
    currentImageIndex = (currentImageIndex + 1) % cursorImages.length;
  }

  // Set interval to change cursor image every second
  setInterval(animateCursor, 1000);

  // Simulate progress update (example: after 2 seconds, update progress to 75%)
  setTimeout(() => {
    currentProgress = 75; // New progress value
    updateProgress(currentProgress);
  }, 2000);

  // Example function to be called when progress needs to be updated
  function setNewProgress(newProgress) {
    currentProgress = newProgress;
    updateProgress(currentProgress);
  }

  // Example usage: Call setNewProgress with new value when needed
  // setNewProgress(80); // This would set progress to 80%
});
