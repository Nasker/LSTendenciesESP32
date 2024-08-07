document.addEventListener('DOMContentLoaded', () => {
  const progressElement = document.querySelector('.progress');
  const progressValueElement = document.getElementById('progress-value');
  const progressPercentageElement = document.getElementById('progress-percentage');
  const progressPhraseElement = document.getElementById('progress-phrase');

  // Function to update progress
  function updateProgress(value, percentage, phrase) {
    progressValueElement.textContent = value;
    progressElement.style.width = percentage + '%';
    progressPercentageElement.textContent = percentage + '%';
    progressPhraseElement.textContent = phrase;
  }

  // Fetch initial progress value from the server
  fetch('/get-progress')
    .then(response => response.json())
    .then(data => {
      updateProgress(data.value, data.percentage, data.phrase);
    });

  // Setup WebSocket connection
  const ws = new WebSocket(`ws://${window.location.hostname}/ws`);

  ws.onopen = () => {
    console.log('WebSocket connection established');
  };

  ws.onmessage = function(event) {
    const data = JSON.parse(event.data);
    updateProgress(data.value, data.percentage, data.phrase);
  };

  ws.onerror = (error) => {
    console.error('WebSocket error:', error);
  };

  ws.onclose = () => {
    console.log('WebSocket connection closed, attempting to reconnect...');
    setTimeout(() => {
      // Try to reconnect every 5 seconds
      ws = new WebSocket(`ws://${window.location.hostname}/ws`);
    }, 2000);
  };
});