//
//  Bridge.h
//  termite_ios
//
//  Created by Milan Toth on 2018. 10. 15..
//

#ifndef Bridge_h
#define Bridge_h

#import <UIKit/UIKit.h>
#import <StoreKit/StoreKit.h>

@interface Bridge : NSObject < SKPaymentTransactionObserver ,
                               SKProductsRequestDelegate >

- ( void ) donate_a_pressed;
- ( void ) donate_b_pressed;
- ( void ) donate_c_pressed;
- ( void ) openurl : ( char* ) url;

@end

#endif /* Bridge_h */

extern Bridge* bridge;
