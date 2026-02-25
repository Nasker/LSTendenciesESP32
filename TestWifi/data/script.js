// Global state
let ledState = false;
let buttonState = false;
let updateInterval;

// Initialize when DOM is loaded
document.addEventListener('DOMContentLoaded', function() {
    console.log('ESP32 Control Panel loaded');
    
    // Initial data fetch
    fetchStatus();
    fetchSystemInfo();
    
    // Start periodic updates
    updateInterval = setInterval(function() {
        fetchStatus();
        fetchSystemInfo();
    }, 1000); // Update every second
});

// Toggle LED via API
function toggleLED() {
    console.log('Toggling LED...');
    
    fetch('/api/led/toggle', {
        method: 'POST'
    })
    .then(response => response.json())
    .then(data => {
        console.log('LED toggle response:', data);
        ledState = data.ledState;
        updateLEDIndicator();
    })
    .catch(error => {
        console.error('Error toggling LED:', error);
        showError('Failed to toggle LED');
    });
}

// Fetch current status (LED and Button)
function fetchStatus() {
    fetch('/api/status')
    .then(response => response.json())
    .then(data => {
        // Update LED state
        if (data.ledState !== ledState) {
            ledState = data.ledState;
            updateLEDIndicator();
        }
        
        // Update Button state
        if (data.buttonState !== buttonState) {
            buttonState = data.buttonState;
            updateButtonIndicator();
        }
    })
    .catch(error => {
        console.error('Error fetching status:', error);
    });
}

// Fetch system information
function fetchSystemInfo() {
    fetch('/api/system')
    .then(response => response.json())
    .then(data => {
        document.getElementById('wifiSsid').textContent = data.ssid || 'Not Connected';
        document.getElementById('ipAddress').textContent = data.ip || '--';
        document.getElementById('rssi').textContent = data.rssi ? data.rssi + ' dBm' : '--';
        document.getElementById('uptime').textContent = formatUptime(data.uptime);
    })
    .catch(error => {
        console.error('Error fetching system info:', error);
    });
}

// Update LED indicator UI
function updateLEDIndicator() {
    const dot = document.getElementById('ledDot');
    const text = document.getElementById('ledText');
    
    if (ledState) {
        dot.classList.add('on');
        dot.classList.remove('off');
        text.textContent = 'ON';
        text.classList.add('on');
        text.classList.remove('off');
    } else {
        dot.classList.remove('on');
        dot.classList.add('off');
        text.textContent = 'OFF';
        text.classList.remove('on');
        text.classList.add('off');
    }
}

// Update Button indicator UI
function updateButtonIndicator() {
    const dot = document.getElementById('buttonDot');
    const text = document.getElementById('buttonText');
    
    if (buttonState) {
        dot.classList.add('active');
        dot.classList.remove('off');
        text.textContent = 'PRESSED';
        text.classList.add('active');
        text.classList.remove('off');
    } else {
        dot.classList.remove('active');
        dot.classList.add('off');
        text.textContent = 'RELEASED';
        text.classList.remove('active');
        text.classList.add('off');
    }
}

// Format uptime in seconds to readable string
function formatUptime(seconds) {
    if (!seconds) return '--';
    
    const days = Math.floor(seconds / 86400);
    const hours = Math.floor((seconds % 86400) / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;
    
    if (days > 0) {
        return `${days}d ${hours}h ${minutes}m`;
    } else if (hours > 0) {
        return `${hours}h ${minutes}m ${secs}s`;
    } else if (minutes > 0) {
        return `${minutes}m ${secs}s`;
    } else {
        return `${secs}s`;
    }
}

// Show error message
function showError(message) {
    const badge = document.getElementById('connectionStatus');
    badge.textContent = message;
    badge.classList.add('disconnected');
    
    setTimeout(() => {
        badge.textContent = 'Connected';
        badge.classList.remove('disconnected');
    }, 3000);
}

// Cleanup on page unload
window.addEventListener('beforeunload', function() {
    if (updateInterval) {
        clearInterval(updateInterval);
    }
});
