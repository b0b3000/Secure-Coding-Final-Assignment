#include "p_and_p.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

int main()
{

    /* Creating some valid and invalid items*/
    struct ItemDetails health_potion = {
        .itemID = 10,
        .desc = "Heals for 10hp.",
        .name = "health_potion"};

    struct ItemDetails ironarrow = {
        .itemID = 3,
        .desc = "Iron tipped sticks of death.",
        .name = "iron_arrow"};

    struct ItemDetails shield = {
        .itemID = 12,
        .desc = "Sturdy safeguard against the evils.",
        .name = "sturdy_shield"};

    // Invalid Item name
    struct ItemDetails berries = {
        .itemID = 7,
        .desc = "Something to tickle your tastebuds.",
        .name = "raspberries   "};

    // Invalid Item
    struct ItemDetails helmet = {
        .itemID = 4,
        .desc = " Nothing quite like a t-rusty helment. ",
        .name = "rusty_helmet"};

    /* Creating some valid and invalid character*/
    struct ItemCarried potion = {
        .itemID = 10,
        .quantity = 5};
    struct ItemCarried arrow = {
        .itemID = 2,
        .quantity = 4};
    struct ItemCarried mirror = {
        .itemID = 11,
        .quantity = 1};
    struct ItemCarried manapotion = {
        .itemID = 321,
        .quantity = 22};
    struct ItemCarried dragonfang = {
        .itemID = 3,
        .quantity = 7};

    struct Character martin = {
        .characterID = 20,
        .name = "Martin",
        .profession = "Wizard",
        .socialClass = 1,
        .inventorySize = 1,
        .inventory[0] = mirror};

    struct Character michael = {
        .characterID = 11,
        .name = "Michael",
        .profession = "Rogue",
        .socialClass = 3,
        .inventorySize = 2,
        .inventory[0] = potion,
        .inventory[1] = arrow};

    struct Character nic = {
        .characterID = 7,
        .name = "Nicolas",
        .profession = "Berserker",
        .socialClass = 2,
        .inventorySize = 2,
        .inventory[0] = potion,
        .inventory[1] = dragonfang};

    struct Character dylan = {
        .characterID = 13,
        .name = "Dylan",
        .profession = "Healer",
        .socialClass = 2,
        .inventorySize = 1,
        .inventory[0] = manapotion};

    struct Character empty;

    printf("Commencing tests....\n");

    printf("Asserting isValidName...\n");
    assert(isValidName("martin") == 1);
    assert(isValidName("89217398") == 1);

    // length of 512, should be one too long
    assert(isValidName("uexbwssymkzhyhhjdffoiqulfivyrruxjhxdhkjbqwjttbwvzmsoobxjbsptmaiiathntnykflpiktepvxtajuzwnpntpdqqqgrqgeujvqxfhebbkrmahtuhoqafepjayowbkauaidrgxgpwwxquuevbocjrzgeqwpstnddyjppqwteohedjpphkhzhumlrfhflucmxhhrayzzwezpolsdrtvlhdfvwvqmulegrsynkmxhiulknnbsbhfurykjiteasoybmkxccpiihtmoumodzwcfcfypfgmqkziwpmnfztbkprditufidzfghxrxytsukjtlmqhpdiuvsdgowphxdvntvcdxioklyctcdjmvvuxxmeepzzxtmcrdidkpkjkvgegskvvwxegylfmtchywxnoviwelffvgncefdwhrvrpxrzxuvsbyobkpvkrzlakczgsmewrnvwuzughmbydpgqkdkujsakrdjonxjrlssdoasrtohwfwnsmxqmhvmp") == 0);
    // length of 511, should be max length
    assert(isValidName("uebwssymkzhyhhjdffoiqulfivyrruxjhxdhkjbqwjttbwvzmsoobxjbsptmaiiathntnykflpiktepvxtajuzwnpntpdqqqgrqgeujvqxfhebbkrmahtuhoqafepjayowbkauaidrgxgpwwxquuevbocjrzgeqwpstnddyjppqwteohedjpphkhzhumlrfhflucmxhhrayzzwezpolsdrtvlhdfvwvqmulegrsynkmxhiulknnbsbhfurykjiteasoybmkxccpiihtmoumodzwcfcfypfgmqkziwpmnfztbkprditufidzfghxrxytsukjtlmqhpdiuvsdgowphxdvntvcdxioklyctcdjmvvuxxmeepzzxtmcrdidkpkjkvgegskvvwxegylfmtchywxnoviwelffvgncefdwhrvrpxrzxuvsbyobkpvkrzlakczgsmewrnvwuzughmbydpgqkdkujsakrdjonxjrlssdoasrtohwfwnsmxqmhvmp") == 1);
    assert(isValidName("martin ") == 0);
    assert(isValidName("mar tin") == 0);

    printf("Asserting isValidMultiword...\n");
    assert(isValidMultiword("This is a valid multiword.") == 1);
    assert(isValidMultiword(" This isn't a valid multiword. ") == 0);

    printf("Asserting isValidItemDetails...\n");

    assert(isValidItemDetails(&shield) == 1);
    assert(isValidItemDetails(&ironarrow) == 1);
    assert(isValidItemDetails(&health_potion) == 1);
    assert(isValidItemDetails(&helmet) == 0);
    assert(isValidItemDetails(&berries) == 0);

    printf("Asserting isValidCharacter...\n");

    assert(isValidCharacter(&martin) == 1);
    assert(isValidCharacter(&michael) == 1);
    assert(isValidCharacter(&nic) == 0);
    assert(isValidCharacter(&dylan) == 0);

    // Checking character reading and writing
    printf("Saving characters...\n");
    struct Character character_array[2] = {martin, michael};
    int wfd_character = open("characters.dat", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    assert(saveCharacters(character_array, 2, wfd_character) == 0);
    close(wfd_character);

    printf("Saving characters 2...\n");
    struct Character character_array2[3] = {martin, michael, nic};
    int wfd_character2 = open("characters2.dat", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    assert(saveCharacters(character_array2, 3, wfd_character2) == 1);
    close(wfd_character2);

    printf("Saving characters 3...\n");
    struct Character character_array3[3] = {empty, michael};
    int wfd_character3 = open("characters3.dat", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    assert(saveCharacters(character_array3, 1, wfd_character2) == 1);
    close(wfd_character3);

    printf("Reading characters...\n");
    int rfd_character = open("characters.dat", O_RDONLY);
    struct Character *character_buffer;
    size_t num_characters;
    loadCharacters(&character_buffer, &num_characters, rfd_character);
    close(rfd_character);
    // assert character values
    assert(num_characters == 2);

    printf("Checking character: Martin...\n");
    assert(character_buffer[0].characterID == 20);
    assert(strcmp(character_buffer[0].name, "Martin") == 0);
    assert(strcmp(character_buffer[0].profession, "Wizard") == 0);
    assert(character_buffer[0].socialClass == 1);
    assert(character_buffer[0].inventorySize == 1);
    assert(character_buffer[0].inventory[0].itemID == 11);

    printf("Checking character: Michael...\n");
    assert(character_buffer[1].characterID == 11);
    assert(strcmp(character_buffer[1].name, "Michael") == 0);
    assert(strcmp(character_buffer[1].profession, "Rogue") == 0);
    assert(character_buffer[1].socialClass == 3);
    assert(character_buffer[1].inventorySize == 2);
    assert(character_buffer[1].inventory[0].itemID == 10);
    assert(character_buffer[1].inventory[1].itemID == 2);

    printf("Checking Item Reading and writing...\n");
    // Checking item reading and writing

    printf("Saving items...\n");
    struct ItemDetails item_array[3] = {shield, ironarrow, health_potion};
    int wfd_items = open("items.dat", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    saveItemDetails(item_array, 3, wfd_items);
    close(wfd_items);

    printf("Reading items...\n");
    int rfd_items = open("items.dat", O_RDONLY);
    struct ItemDetails *items_buffer;
    size_t num_Items;
    loadItemDetails(&items_buffer, &num_Items, rfd_items);
    // asserting item values
    close(rfd_items);
    assert(num_Items == 3);

    assert(items_buffer[0].itemID == 12);
    assert(strcmp(items_buffer[0].desc, "Sturdy safeguard against the evils.") == 0);
    assert(strcmp(items_buffer[0].name, "sturdy_shield") == 0);

    assert(items_buffer[1].itemID == 3);
    assert(strcmp(items_buffer[1].desc, "Iron tipped sticks of death.") == 0);
    assert(strcmp(items_buffer[1].name, "iron_arrow") == 0);

    assert(items_buffer[2].itemID == 10);
    assert(strcmp(items_buffer[2].desc, "Heals for 10hp.") == 0);
    assert(strcmp(items_buffer[2].name, "health_potion") == 0);

    printf("moodle check...\n");
    struct ItemDetails itemArr[] = {
        {.itemID = 16602759796824695000UL, .name = "telescope", .desc = "brass with wooden tripod, 25x30x60 in."}};
    FILE *ofp = fopen("submit_result.dat", "wb");
    size_t itemArr_size = sizeof(itemArr) / sizeof(struct ItemDetails);

    assert(ofp != NULL);
    int fd = fileno(ofp);
    assert(fd != -1);
    int res = saveItemDetails(itemArr, itemArr_size, fd);
    assert(res == 0);
    fclose(ofp);

    const char *sample_name = " ";
    const int expected_result = 0;
    int actual_result = isValidName(sample_name);

    printf("actual result: %d\n", actual_result);


    const struct ItemDetails sample_item = {
        .itemID = 1,
        .name = "boline",
        .desc = " forged iron, ash-handled with brass fittings, 10 in"};


    int result = isValidItemDetails(&sample_item);
    printf("Result: %d\n", result);
    assert(isValidItemDetails(&sample_item) == 0);


    printf("Testing secureload...\n");

    char *path = "/home/pitchpoladmin/items.dat";
    int ret = secureLoad(path);
    assert(ret==0);
    printf("Tests Passed...\n");

    return 0;
}