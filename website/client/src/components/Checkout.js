import React from 'react'
import StripeCheckout from 'react-stripe-checkout'
import { useDispatch } from 'react-redux';
import { placeOrder } from '../actions/orderActions';
export default function Checkout({subtotal}) {
    const dispatch = useDispatch()
    function tokenHandler(token){
        console.log(token);
        dispatch(placeOrder(token, subtotal))

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

