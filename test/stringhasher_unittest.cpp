#include "StringHasher.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

const int A = 900015709;
const int M = 1000000007;

TEST(StringHasher, Default) {
  // construct
  std::string text = R"ZZZ(If I speak in the tongues of men and of angels, but have not love, I am a noisy gong or a clanging cymbal. And if I have prophetic powers, and understand all mysteries and all knowledge, and if I have all faith, so as to remove mountains, but have not love, I am nothing. If I give away all I have, and if I deliver up my body to be burned,[a] but have not love, I gain nothing.

Love is patient and kind; love does not envy or boast; it is not arrogant or rude. It does not insist on its own way; it is not irritable or resentful;[b] it does not rejoice at wrongdoing, but rejoices with the truth. Love bears all things, believes all things, hopes all things, endures all things.

Love never ends. As for prophecies, they will pass away; as for tongues, they will cease; as for knowledge, it will pass away. For we know in part and we prophesy in part, but when the perfect comes, the partial will pass away. When I was a child, I spoke like a child, I thought like a child, I reasoned like a child. When I became a man, I gave up childish ways. For now we see in a mirror dimly, but then face to face. Now I know in part; then I shall know fully, even as I have been fully known.

So now faith, hope, and love abide, these three; but the greatest of these is love.)ZZZ";
  snap::StringHasher hasher(text, M, A);
  ASSERT_EQ(A, hasher.A);
  ASSERT_EQ(M, hasher.M);
  ASSERT_EQ(text, hasher.text);  
  ASSERT_EQ(651994891, hasher.hash(-4, 2)); // If 
  ASSERT_EQ(116, hasher.hash(18, 18)); // t
  ASSERT_EQ(901743122, hasher.hash(18, 19)); // to
  ASSERT_EQ(979240235, hasher.hash(18, 24)); // tongues
  ASSERT_EQ(hasher.hash(18, 24), hasher.hash(746, 752)); // tongues
  ASSERT_EQ(426864576, hasher.hash(1261, 99999)); // love.
  ASSERT_EQ(10, hasher.hash(681, 681)); // \n
  ASSERT_EQ(838624455, hasher.hash(681, 686)); // \nLove 
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();    
}
