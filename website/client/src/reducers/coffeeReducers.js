export const getAllCoffeesReducer=(state={coffees : []}, action)=>{
    switch(action.type){
        case 'GET_COFFEES_REQUEST': return{
            loading : true,
            ...state
        }
        case 'GET_COFFEES_SUCCESS': return{
            loading : false,
            coffees: action.payload
        }
        case 'GET_COFFEES_FAILED': return{
            error: action.payload,
            loading: false
        }
        default: return state
    }
}