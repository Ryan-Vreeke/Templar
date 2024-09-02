fetch('http://localhost:8000/api/')
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.json(); // parse the JSON from the response
    })
    .then(data => {
        console.log(data); // handle the JSON data
    })
    .catch(error => {
        console.error('There was a problem with the fetch operation:', error);
    });
