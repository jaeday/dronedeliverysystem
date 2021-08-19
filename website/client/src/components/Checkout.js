import React from 'react'
import StripeCheckout from 'react-stripe-checkout'

export default function Checkout({subtotal}) {
    function tokenHandler(token){
        console.log(token);
    }
    return (
        <div>
            <StripeCheckout amount = {subtotal*100}
            shippingAddress
            token = {tokenHandler}
            stripeKey='pk_test_51JPoR1GdJegYvYHwIRKAK9f2zXMvGwggeC469uPdTIT9X1AnIcFtAp8T3SMiuW8iRDsCpAZyixeBzYOCdTi3ttDq00ZyGvGI73'>
                <button className = 'btn'>
                    Pay Now
                </button>
            </StripeCheckout>
        </div>
    )
}

