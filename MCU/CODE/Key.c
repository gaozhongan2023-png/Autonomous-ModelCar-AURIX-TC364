/*********************************************************************************************************************
* @file             KEY.c
* @author           Andreas WH
* @brief            按键的初始化与读取
* @Target core      TC364
* @board            TC364_MainBoard_V1.1
* @date             2021-12-26
********************************************************************************************************************/

#include "KEY.h"
#include "headfile.h"

/* 类型定义 */

/* 按键引脚定义 */
static uint32 GPIO_PIN_KEY[KEYn] = {KEY_UP_PIN, KEY_DOWN_PIN, KEY_LEFT_PIN, KEY_RIGHT_PIN, KEY_SET_PIN, KEY_BACK_PIN};

/* 拨码开关引脚定义 */
static uint32 GPIO_PIN_SWITCH[KEYn] = {SWITCH1_PIN, SWITCH2_PIN, SWITCH3_PIN, SWITCH4_PIN};


//-------------------------------------------------------------------------------------------------------------------
// @brief       按键初始化
// @param       keynum              选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      void
// Sample usage:            KEY_Init(KEY_UP);//初始化KEY_UP
//-------------------------------------------------------------------------------------------------------------------
void KEY_Init(key_typedef_enum keynum)
{
    if(keynum != KEY_ALL)
    {
        /* 配置KEY GPIO引脚 */
        gpio_init(GPIO_PIN_KEY[keynum], GPI, 1, PULLUP);
    }
    else
    {
        gpio_init(GPIO_PIN_KEY[0], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_KEY[1], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_KEY[2], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_KEY[3], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_KEY[4], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_KEY[5], GPI, 1, PULLUP);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取单个按键输入状态
// @param       keynum              选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      uint8
// Sample usage:            if(KEY_PRESS == KEY_Get_Input(KEY_UP)) LED_On(LED1);//如果UP键按下，点亮LED1
//-------------------------------------------------------------------------------------------------------------------
uint8 KEY_Get(key_typedef_enum keynum)
{
    return gpio_get(GPIO_PIN_KEY[keynum]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取单个按键输入状态
// @param       keynum              选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      uint8
// Sample usage:            if(KEY_PRESS == KEY_Get_Input(KEY_UP)) LED_On(LED1);//如果UP键按下，点亮LED1
//-------------------------------------------------------------------------------------------------------------------
uint8 KEY_Get_Input(key_typedef_enum keynum)
{
    systick_delay_ms(STM0,20);
    return gpio_get(GPIO_PIN_KEY[keynum]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       读取输入的按键值
// @param       mode                读取模式
// @return      uint8
// Sample usage:            if(1 == KEY_Read(1)) LED_On(LED1);//如果读取的按键值为1（up键），点亮LED1
//-------------------------------------------------------------------------------------------------------------------
uint8 KEY_Read(uint8 mode)
{
    static uint8 key_up=1;     //按键松开标志
    if(mode==1)
    {
        key_up=1;      //支持连按
    }
    if(key_up && (KEY_PRESS == KEY_Get_Input(KEY_UP) || KEY_PRESS == KEY_Get_Input(KEY_DOWN) || KEY_PRESS == KEY_Get_Input(KEY_LEFT)||
                  KEY_PRESS == KEY_Get_Input(KEY_RIGHT) || KEY_PRESS == KEY_Get_Input(KEY_SET) || KEY_PRESS == KEY_Get_Input(KEY_BACK)))
    {
        systick_delay_ms(STM0, 20);  //消抖
        key_up=0;
        if(KEY_PRESS == KEY_Get_Input(KEY_UP)) return 1;            //up
        else if(KEY_PRESS == KEY_Get_Input(KEY_LEFT)) return 2;     //left
        else if(KEY_PRESS == KEY_Get_Input(KEY_RIGHT)) return 3;    //right
        else if(KEY_PRESS == KEY_Get_Input(KEY_DOWN)) return 4;     //down
        else if(KEY_PRESS == KEY_Get_Input(KEY_SET)) return 5;      //set
        else if(KEY_PRESS == KEY_Get_Input(KEY_BACK)) return 6;     //back
    }
    if((KEY_RELEASE == KEY_Get_Input(KEY_UP) || KEY_RELEASE == KEY_Get_Input(KEY_DOWN) || KEY_RELEASE == KEY_Get_Input(KEY_LEFT)||
        KEY_RELEASE == KEY_Get_Input(KEY_RIGHT) || KEY_RELEASE == KEY_Get_Input(KEY_SET) || KEY_RELEASE == KEY_Get_Input(KEY_BACK)))
    {
        key_up=1;
    }
    return 0;   //无按键按下
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       按键初始化
// @param       switchnum               选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      void
// Sample usage:            SWITCH_Init(SWITCH1);//初始化拨码开关1
//-------------------------------------------------------------------------------------------------------------------
void SWITCH_Init(switch_typedef_enum switchnum)
{
    if(switchnum != SWITCH_ALL)
    {
        /* 配置SWITCH GPIO引脚 */
        gpio_init(GPIO_PIN_SWITCH[switchnum], GPI, 1, PULLUP);
    }
    else
    {
        gpio_init(GPIO_PIN_SWITCH[0], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_SWITCH[1], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_SWITCH[2], GPI, 1, PULLUP);
        gpio_init(GPIO_PIN_SWITCH[3], GPI, 1, PULLUP);
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       获取单个按键输入状态
// @param       keynum              选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      uint8
// Sample usage:            if(SWITCH_ON == SWITCH_Get_Input(SWITCH)) LED_On(LED1);//如果拨码开关1打开，点亮LED
//-------------------------------------------------------------------------------------------------------------------
uint8 SWITCH_Get_Input(switch_typedef_enum switchnum)
{
    return gpio_get(GPIO_PIN_SWITCH[switchnum]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       读取输入的拨码开关值
// @return      uint8
// Sample usage:            if(1 == SWITCH_Read()) LED_On(LED1);//如果读取的按键值为1（up键），点亮LED1
//-------------------------------------------------------------------------------------------------------------------
uint8 SWITCH_Read(void)
{
    uint8 value = 0;
    if(SWITCH_ON == SWITCH_Get_Input(SWITCH1)) value |= 1;          //1
    if(SWITCH_ON == SWITCH_Get_Input(SWITCH2)) value |= (1<<1);     //2
    if(SWITCH_ON == SWITCH_Get_Input(SWITCH3)) value |= (1<<2);     //3
    if(SWITCH_ON == SWITCH_Get_Input(SWITCH4)) value |= (1<<3);     //4
    return value;
}
