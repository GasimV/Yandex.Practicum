const siteUrl = 'https://127.0.0.1:8000/';
const styleUrl = siteUrl + 'static/css/bookmarklet.css';
const minWidth = 250;
const minHeight = 250;

// Load CSS
var head = document.getElementsByTagName('head')[0];
var link = document.createElement('link');
link.rel = 'stylesheet';
link.type = 'text/css';
link.href = styleUrl + '?r=' + Math.floor(Math.random() * 9999999999999999);
head.appendChild(link);

// Load HTML
var body = document.getElementsByTagName('body')[0];
var boxHtml = `
    <div id="bookmarklet">
        <a href="#" id="close">&times;</a>
        <h1>Select an image to bookmark:</h1>
        <div class="images"></div>
    </div>`;
body.innerHTML += boxHtml;

function bookmarkletLaunch() {
    var bookmarklet = document.getElementById('bookmarklet');
    var imagesFound = bookmarklet.querySelector('.images');
    // Clear images found
    imagesFound.innerHTML = '';
    // Display bookmarklet
    bookmarklet.style.display = 'block';
    // Close event
    bookmarklet.querySelector('#close').addEventListener('click', function() {
        bookmarklet.style.display = 'none';
    });

    // Find images in the DOM with the minimum dimensions
    var images = document.querySelectorAll('img[src$=".jpg"], img[src$=".jpeg"], img[src$=".png"]');
    console.log('Found images:', images);  // Debugging statement
    images.forEach(image => {
        console.log('Image:', image.src, 'Width:', image.naturalWidth, 'Height:', image.naturalHeight);  // Debugging statement
        if (image.naturalWidth >= minWidth && image.naturalHeight >= minHeight) {
            var imageFound = document.createElement('img');
            imageFound.src = image.src;
            imagesFound.append(imageFound);
            console.log('Added image:', image.src);  // Debugging statement
        }
    });

    // Select image event
    imagesFound.querySelectorAll('img').forEach(image => {
        image.addEventListener('click', function(event){
            var imageSelected = event.target;
            bookmarklet.style.display = 'none';
            window.open(siteUrl + 'images/create/?url='
                        + encodeURIComponent(imageSelected.src)
                        + '&title='
                        + encodeURIComponent(document.title),
                        '_blank');
        });
    });
}

// Launch the bookmarklet
bookmarkletLaunch();