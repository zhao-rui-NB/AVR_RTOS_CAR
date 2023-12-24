HEX_PATH = rf"C:\Users\rui\Desktop\AVR_Tools\SER5_2\default\SER5_2.hex"

import time
from pywinauto.application import Application

app = Application(backend="uia").start("avrubd/avrubd.exe")
res = ''
try:
    arr = app.windows()
    w = app.AVR_Universal_Bootloader_Download
    w.SplitButton.click_input()

    # app2['&Load']['ComboBox2'].print_control_identifiers()
    # app2['&Load'].type_keys(rf"C:\Users\rui\Desktop\AVR_Tools\SER5_2\default\SER5_2.hex")
    # app2['&Load'].type_keys("{ENTER}")

    # app['&Load'].print_control_identifiers()
    app['&Load']['檔案名稱(N):2'].edit.set_text(HEX_PATH)
    app['&Load']['開啟(O)'].click_input()

    ti = app.windows()[0].texts()[0]
    w = app[ti]
    w.Button2.click_input()

    # get text from panel
    
    res = w.Pane9.TabControlLog.Log.children()[1].window_text()
    # wait w.Pane9.TabControlLog.Log.children()[1].window_text() not change
    while True:
        time.sleep(0.05)
        res2 = w.Pane9.TabControlLog.Log.children()[1].window_text()
        if res2 == res:
            break
        else:
            res = res2
    
    print('####### res #######')
    print(res)
except Exception as e:
    print('#######FILE OPEN ERROR #######')

app.kill()


print()
print()
if 'fail' in res:
    print('##################################')
    print('############## FAIL ##############')
    print('##################################')
elif 'Update success' in res:
    print('##################################')
    print('############# SUCCESS ############')
    print('##################################')
