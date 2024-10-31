
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.firefox.service import Service
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.firefox import GeckoDriverManager
import time

# Set up the WebDriver (for Firefox in this case)
driver = webdriver.Firefox(service=Service(GeckoDriverManager().install()))

# Navigate to the website
driver.get('https://letsmakeagame.net/planet-name-generator/')

# Number of names to generate
number_of_names = 500

# List to store the generated names
generated_names = set()

# Explicit wait
wait = WebDriverWait(driver, 1)

# Switch to the iframe by its title attribute
iframe_title = "Planet Name Generator"  # Replace with the actual title of the iframe
iframe = wait.until(EC.frame_to_be_available_and_switch_to_it((By.XPATH, f'//iframe[@title="{iframe_title}"]')))

while len(generated_names) < number_of_names:
    # Wait for the 'Generate' button to be clickable and click it
    generate_button = wait.until(EC.element_to_be_clickable((By.XPATH, '//button[contains(text(), "GENERATE")]')))
    generate_button.click()

    # Find and store the generated name
    planet_name = wait.until(EC.presence_of_element_located((By.XPATH, '//p[@id="title"]'))).text
    generated_names.add(planet_name)

# Switch back to default content
driver.switch_to.default_content()

# Output the names to a file
with open('generated_names.txt', 'w') as file:
    for name in generated_names:
        file.write(f"{name}\n")

# Close the browser
driver.quit()
