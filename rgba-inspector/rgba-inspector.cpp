#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cmath>

#include <SFML/Graphics.hpp>

int getCoefficient(float x)
{
    return (x >= 0.f) ? 1
                      : -1;
}

float normalizePixelOffset(float value)
{
    // Convert the result of sine or cosine to -1, 0, or 1.
    return ceil(fabs(value)) * getCoefficient(value);
}

sf::Vector2i getNeighbourPixelOffset(int theta)
{
    float x = cos(theta);
    float y = sin(theta);

    x = (int) normalizePixelOffset(x);
    y = (int) normalizePixelOffset(y);

    return sf::Vector2i(x, y);
}

int main()
{
    int windowWidth = 1152;
    int windowHeight = 648;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "RGBA Inspector by Sean Francis N. Ballais", sf::Style::Titlebar | sf::Style::Close);
    
    sf::Texture inspecteeTexture;
    sf::Sprite inspecteeSprite;

    if (!inspecteeTexture.loadFromFile("test.png", sf::IntRect(0, 0, 800, 600)))
    {
        std::cout << "Could not load test image." << std::endl;
    }

    inspecteeSprite.setTexture(inspecteeTexture);
    inspecteeSprite.setPosition((windowWidth/ 2) - 400, (windowHeight / 2) - 300);

    // Set up the views.
    sf::View inspecteeView(sf::FloatRect(0.f, 0.f, (float) windowWidth, (float) windowHeight));
    inspecteeView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
    
    float inspecteeViewZoomAmount = 1.f;
    float inspecteeViewZoomFactor = 25.f;
    int inspecteeViewZoomLimit = 100;
    int inspecteeViewZoomLevel = 0;
    int previousInspecteeViewZoomLevel = 0;
    float newInspecteeViewXOffset = 0.f;
    float newInspecteeViewYOffset = 0.f;

    // TODO: Add information box.

    float inspecteeViewMoveOffsetFactor = 10.f;

    sf::RectangleShape mainPixelMarker(sf::Vector2f(1.f, 1.f));
    sf::RectangleShape neighbourPixelMarker(sf::Vector2f(20.f, 20.f));

    sf::View informationView(sf::FloatRect(0.f, 0.f, (float) windowWidth, (float) windowHeight));
    informationView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

    sf::RectangleShape informationBox(sf::Vector2f(200.f, 450.f));
    informationBox.setFillColor(sf::Color(149, 165, 166, 191));
    informationBox.move(50.f, 0.f);

    int neighbourAngle = 0;

    // Mouse position coordinates.
    float mouseX = 0.f;
    float mouseY = 0.f;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (   (event.type == sf::Event::Closed)
                || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
               )
            {
                window.close();
            }

            // Handle keyboard events

            if (event.type == sf::Event::KeyPressed)
            {
                float adaptiveMovementOffset = inspecteeViewMoveOffsetFactor - (inspecteeViewZoomLevel / inspecteeViewMoveOffsetFactor);
                adaptiveMovementOffset = std::max(adaptiveMovementOffset, 1.f);

                if (event.key.code == sf::Keyboard::W)
                {
                    newInspecteeViewYOffset = -adaptiveMovementOffset;
                }
                
                if (event.key.code == sf::Keyboard::S)
                {
                    newInspecteeViewYOffset = adaptiveMovementOffset;
                }
                
                if (event.key.code == sf::Keyboard::A)
                {
                    newInspecteeViewXOffset = -adaptiveMovementOffset;
                }
                
                if (event.key.code == sf::Keyboard::D)
                {
                    newInspecteeViewXOffset = adaptiveMovementOffset;
                }

                if (event.key.code == sf::Keyboard::Q)
                {
                    neighbourAngle = (neighbourAngle + 45) % 360;
                }

                if (event.key.code == sf::Keyboard::E)
                {
                    neighbourAngle = (neighbourAngle - 45) % 360;
                }
            }

            // Handle mouse events.
            mouseX = sf::Mouse::getPosition(window).x;
            mouseY = sf::Mouse::getPosition(window).y;

            std::cout << "Mouse X Position: " << mouseX << std::endl;
            std::cout << "Mouse Y Position: " << mouseY << std::endl;

            if (event.type == sf::Event::MouseWheelScrolled)
            {
                std::cout << "Zoom Amount: " << inspecteeViewZoomAmount << std::endl;
                std::cout << "Scroll delta: " << (float) event.mouseWheelScroll.delta / 25.f << std::endl;
                std::cout << "Zoom Limit: " << inspecteeViewZoomLimit << std::endl;
                std::cout << "Zoom Level: " << inspecteeViewZoomLevel << std::endl;

                inspecteeViewZoomLevel += event.mouseWheelScroll.delta;               

                if (abs(inspecteeViewZoomLevel) <= inspecteeViewZoomLimit)
                {
                    // When scrolling down, the image will be zoomed out. Zooming out requires setting the
                    // zoom amount to be greater than 1 since the inspecteeView (and SFML views, in general)
                    // requires a number greater than 1 to zoom out (the object actually becomes smaller).
                    inspecteeViewZoomAmount -= (float) event.mouseWheelScroll.delta / 25.f;
                }
            }
        }

        // Update objects.
        sf::Vector2f mainMarkerViewPosition = window.mapPixelToCoords(sf::Mouse::getPosition(window), inspecteeView);
        mainPixelMarker.setPosition(sf::Vector2f(mainMarkerViewPosition.x, mainMarkerViewPosition.y));

        sf::Vector2i neighbourPixelOffset = getNeighbourPixelOffset(neighbourAngle);
        sf::Vector2i neighbourPixelMarkerPosition(sf::Mouse::getPosition(window).x + neighbourPixelOffset.x,
                                                  sf::Mouse::getPosition(window).y + neighbourPixelOffset.y);

        std::cout << "neighbourPixel X: " << neighbourPixelMarkerPosition.x << std::endl;
        std::cout << "neighbourPixel Y: " << neighbourPixelMarkerPosition.y << std::endl;

        sf::Vector2f neighbourMarkerViewPosition = window.mapPixelToCoords(neighbourPixelMarkerPosition, inspecteeView);
        neighbourPixelMarker.setPosition(sf::Vector2f(neighbourMarkerViewPosition.x, neighbourMarkerViewPosition.y));

        // Draw objects.
        window.clear();

        // Draw the stuff in the image view first.
        window.setView(inspecteeView);

        inspecteeView.move(newInspecteeViewXOffset, newInspecteeViewYOffset);
        inspecteeView.zoom(inspecteeViewZoomAmount);

        inspecteeViewZoomAmount = 1.f;

        if (inspecteeViewZoomLevel > 0)
        {
            inspecteeViewZoomLevel = std::min(inspecteeViewZoomLevel, inspecteeViewZoomLimit);
        }
        else
        {
            inspecteeViewZoomLevel = std::max(inspecteeViewZoomLevel, -inspecteeViewZoomLimit);
        }

        newInspecteeViewXOffset = 0.f;
        newInspecteeViewYOffset = 0.f;

        window.draw(inspecteeSprite);
        window.draw(mainPixelMarker);
        window.draw(neighbourPixelMarker);

        // Draw the stuff in the pixel marker view.
        window.setView(informationView);
        window.draw(informationBox);

        window.display();
    }

    return 0;
}