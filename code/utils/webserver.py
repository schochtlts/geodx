import http.server
import socketserver
import os

# Set the directory where your files are located
DIRECTORY = "../../"

# Set the port number
PORT = 8000

class MyHttpRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        # Set the content type based on the file extension
        if self.path.endswith(".html"):
            self.send_response(200)
            self.send_header("Content-type", "text/html")
        elif self.path.endswith(".js"):
            self.send_response(200)
            self.send_header("Content-type", "application/javascript")
        elif self.path.endswith(".wasm"):
            self.send_response(200)
            self.send_header("Content-type", "application/wasm")
        elif self.path.endswith(".data"):
            self.send_response(200)
        else:
            # Serve other file types as well
            super().do_GET()
            return

        try:
            # Open the requested file and send its content
            with open(os.path.join(DIRECTORY, self.path[1:]), "rb") as file:
                self.end_headers()
                self.wfile.write(file.read())
        except FileNotFoundError:
            self.send_error(404, "File Not Found: {}".format(self.path))

# Set up the HTTP server
with socketserver.TCPServer(("", PORT), MyHttpRequestHandler) as httpd:
    print("Server running on port", PORT)
    # Start the server
    httpd.serve_forever()
