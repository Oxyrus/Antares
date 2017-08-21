#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

constexpr int windowWidth{800}, windowHeight{600};
constexpr float ballRadius{10.f}, ballVelocity{8.f};
constexpr float paddleWidth{60.f}, paddleHeight{20.f}, paddleVelocity{6.f};

constexpr float blockWidth{60.f}, blockHeight{20.f};
constexpr int countBlocksX{11}, countBlocksY{4};

struct Ball {
    sf::CircleShape shape;
    sf::Vector2f velocity{-ballVelocity, -ballVelocity};

    Ball(float mX, float mY) {
        shape.setPosition(mX, mY);
        shape.setRadius(ballRadius);
        shape.setFillColor(sf::Color::Cyan);
        shape.setOrigin(ballRadius, ballRadius);
    }

    void update() {
        shape.move(velocity);

        // Left/Right collisions
        if (left() < 0) velocity.x = ballVelocity;
        else if (right() > windowWidth) velocity.x = -ballVelocity;

        // Top/Bottom collisions
        if(top() < 0) velocity.y = ballVelocity;
        else if(bottom() > windowHeight) velocity.y = -ballVelocity;
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getRadius(); }
    float right() { return x() + shape.getRadius(); }
    float top() { return y() - shape.getRadius(); }
    float bottom() { return y() + shape.getRadius(); }
};

struct Paddle {
    sf::RectangleShape shape;
    sf::Vector2f velocity;

    Paddle(float mX, float mY) {
        shape.setPosition(mX, mY);
        shape.setSize({paddleWidth, paddleHeight});
        shape.setFillColor(sf::Color::Green);
        shape.setOrigin(paddleWidth / 2, paddleHeight / 2);
    }

    void update() {
        shape.move(velocity);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > 0) {
            velocity.x = -paddleVelocity;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < windowWidth) {
            velocity.x = paddleVelocity;
        }
        else {
            velocity.x = 0;
        }
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

struct Brick {
    sf::RectangleShape shape;
    bool destroyed{false};

    Brick(float mX, float mY) {
        shape.setPosition(mX, mY);
        shape.setSize({blockWidth, blockHeight});
        shape.setFillColor(sf::Color::Magenta);
        shape.setOrigin(blockWidth / 2.f, blockHeight / 2.f);
    }

    float x() { return shape.getPosition().x; }
    float y() { return shape.getPosition().y; }
    float left() { return x() - shape.getSize().x / 2.f; }
    float right() { return x() + shape.getSize().x / 2.f; }
    float top() { return y() - shape.getSize().y / 2.f; }
    float bottom() { return y() + shape.getSize().y / 2.f; }
};

template <class T1, class T2> bool isIntersecting(T1& mA, T2& mB) {
    return mA.right() >= mB.left()
           && mA.left() <= mB.right()
           && mA.bottom() >= mB.top()
           && mA.top() <= mB.bottom();
};

void testCollision(Paddle& mPaddle, Ball& mBall) {
    if (!isIntersecting(mPaddle, mBall)) return;

    // Push the ball upwards
    mBall.velocity.y = -ballVelocity;

    if (mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity;
    else mBall.velocity.x = ballVelocity;
}

void testCollision(Brick& mBrick, Ball& mBall) {
    if (!isIntersecting(mBrick, mBall)) return;

    mBrick.destroyed = true;

    float overlapLeft{mBall.right() - mBrick.left()};
    float overlapRight{mBrick.right() - mBall.left()};
    float overlapTop{mBall.bottom() - mBrick.top()};
    float overlapBottom{mBrick.bottom() - mBall.top()};

    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));
    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

    float minOverlapX{ballFromLeft ? overlapLeft : overlapRight};
    float minOverlapY{ballFromTop ? overlapTop : overlapBottom};

    if (abs(minOverlapX) < abs(minOverlapY)) {
        mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
    }
    else {
        mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
    }
}

int main() {
    // Create window and set FPS limit
    sf::RenderWindow window{{windowWidth, windowHeight}, "Antares"};
    window.setFramerateLimit(30);

    Ball ball{windowWidth / 2, windowHeight / 2};
    Paddle paddle{windowWidth / 2, windowHeight - 20};

    std::vector<Brick> bricks;
    for (int iX{0}; iX < countBlocksX; ++iX) {
        for (int iY{0}; iY < countBlocksY; ++iY) {
            bricks.emplace_back((iX + 1) * (blockWidth + 3) + 22,
                                (iY + 2) * (blockHeight + 3));
        }
    }

    while (true) {
        // Prevents the window from freezing
        sf::Event event;
        window.pollEvent(event);

        window.clear(sf::Color::Black);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) break;

        ball.update();
        paddle.update();

        window.draw(ball.shape);
        window.draw(paddle.shape);

        for (auto& brick : bricks) testCollision(brick, ball);
        bricks.erase(std::remove_if(std::begin(bricks), std::end(bricks),
                     [](const Brick& mBrick) { return mBrick.destroyed; }),
                     std::end(bricks));

        testCollision(paddle, ball);

        for (auto& brick : bricks) window.draw(brick.shape);
        window.display();
    }

    return 0;
}
