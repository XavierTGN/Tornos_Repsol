from PIL import Image
import base64
from io import BytesIO

# Abre la imagen (ajusta el nombre si es diferente)
with Image.open("repsol_1.jpg") as img:
    # Opcional: redimensionar
    img = img.resize((200, 100))
    buffered = BytesIO()
    img.save(buffered, format="JPEG")
    img_base64 = base64.b64encode(buffered.getvalue()).decode("utf-8")

# Muestra el HTML listo para usar en el ESP32
print("<img src='data:image/jpeg;base64," + img_base64 + "' alt='Repsol Logo' width='200'><br>")
